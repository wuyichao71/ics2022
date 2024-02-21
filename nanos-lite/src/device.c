#include <common.h>
/* wuyc */
#include <fs.h>
/* wuyc */

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

/* wuyc */
void set_fg_pcb(int index);
/* wuyc */

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  /* yield(); */
  for (int i = 0; i < len; i++)
  {
    putch(((char *)buf)[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  /* yield(); */
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  /* printf("%d\n", AM_INPUT_KEYBRD); */
  if (ev.keycode != AM_KEY_NONE)
  {
    len = sprintf(buf, "k%c %s\n", ev.keydown ? 'd' : 'u', keyname[ev.keycode]);
    /* printf("%d\n", AM_INPUT_KEYBRD); */
    switch (ev.keycode)
    {
      case AM_KEY_F1: set_fg_pcb(1); break;
      case AM_KEY_F2: set_fg_pcb(2); break;
      case AM_KEY_F3: set_fg_pcb(3); break;
    }
    return len;
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  /* yield(); */
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width, height = cfg.height;
  len = sprintf(buf, "WIDTH: %d\nHEIGHT: %d\n", width, height);
  return len;
  /* return 0; */
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int fd = fs_open("/dev/fb", 0, 0);
  int size = fs_lseek(fd, 0, SEEK_END) / sizeof(uint32_t);
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  offset = offset / sizeof(uint32_t);
  len = len / sizeof(uint32_t);
  int y = offset / cfg.width, x = offset % cfg.width;
  int newoffset = offset + len;
  /* printf("%d\n", len); */
  if (newoffset > size)
  {
    len = size - offset;
    newoffset = size;
    /* printf("len = %d, newoffset = %d\n", len, newoffset); */
  }
  if (x + len <= cfg.width)
  {
    io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len, 1, true);
    /* printf("In part 1\n"); */
  }
  else
  {
    if (x != 0)
    {
      int rest = cfg.width - x;
      if (rest != 0)
        io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, rest, 1, true);
      buf += rest * sizeof(uint32_t);
      len -= rest;
      y += 1;
      x = 0;
      /* printf("In part 2\n"); */
    }
    int h = len / cfg.width, rem = len % cfg.width;
    io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, cfg.width, h, true);
    buf += cfg.width * h * sizeof(uint32_t);
    y += h;
    if (rem != 0)
      io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, rem, 1, true);
    /* printf("In part 3\n"); */
  }
  /* set_variable(new_offset); */
  /* printf("%d\n%d\n", len, offset); */
  fs_lseek(fd, newoffset * sizeof(uint32_t), SEEK_SET);
  fs_close(fd);
  return len;
}

/* size_t sb_write() */
size_t sbctl_write(const void *buf, size_t offset, size_t len)
{
  assert(len == 3 * sizeof(uint32_t));
  io_write(AM_AUDIO_CTRL, ((uint32_t *)buf)[0], ((uint32_t *)buf)[1], ((uint32_t *)buf)[2]);
  return len;
}

size_t sbctl_read(void *buf, size_t offset, size_t len)
{
  assert(len == sizeof(int));
  int count = io_read(AM_AUDIO_STATUS).count;
  int bufsize = io_read(AM_AUDIO_CONFIG).bufsize;
  ((int *)buf)[0] = bufsize - count;
  return len;
}

size_t sb_write(void *buf, size_t offset, size_t len)
{
  /* int count; */
  /* do */
  /* { */
    /* count = io_read(AM_AUDIO_STATUS).count; */
  /* } while (count < len); */
  Area sbuf;
  sbuf.start = buf;
  sbuf.end = sbuf.start + len;
  io_write(AM_AUDIO_PLAY, sbuf);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
