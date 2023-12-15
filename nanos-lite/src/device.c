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

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; i++)
  {
    putch(((char *)buf)[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  /* printf("%d\n", AM_INPUT_KEYBRD); */
  if (ev.keycode != AM_KEY_NONE)
  {
    len = sprintf(buf, "k%c %s\n", ev.keydown ? 'd' : 'u', keyname[ev.keycode]);
    /* printf("%d\n", AM_INPUT_KEYBRD); */
    return len;
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width, height = cfg.height;
  len = sprintf(buf, "WIDTH: %d\nHEIGHT: %d\n", width, height);
  return len;
  /* return 0; */
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int fd = fs_open("/dev/fb", 0, 0);
  int size = fs_lseek(fd, 0, SEEK_END);
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int y = offset / cfg.width, x = offset % cfg.width;
  /* printf("%d\n", size); */
  if (offset + len > size)
    len = size - offset;
  if (x + len > cfg.width)
  {
    int rest = cfg.width - x;
    io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, rest, 1, true);
    buf += rest;
    len -= rest;
    y += 1;
    x = 0;
    int h = len / cfg.width;
    int rem = len % cfg.width;
    if (h > 0)
    {
      io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, cfg.width, h, true);
      y += h;
    }
    buf += (cfg.width * h);
    io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, rem, 1, true);
  }
  else
  {
    io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len, 1, true);
  }
  /* set_variable(new_offset); */
  /* printf("%d\n%d\n", len, offset); */
  /* fs_lseek(fd, 0, SEEK_SET); */
  fs_close(fd);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
