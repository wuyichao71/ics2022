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

#define set_variable(variable) { \
  variable = finfo.open_offset + len; \
  if (variable > finfo.size) \
  { \
    variable = finfo.size; \
    len = finfo.size - finfo.open_offset; \
  } \
} while(0)

size_t fb_write(const void *buf, size_t offset, size_t len) {
  /* AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG); */
  int fd = fs_open("/dev/fb", 0, 0);
  int size = fs_lseek(fd, 0, SEEK_END);
  printf("%d\n", size);
  /* int x = offset / cfg.width, y = offset % cfg.width; */
  /* int new_offset; */
  /* set_variable(new_offset); */
  /* printf("%d\n%d\n", len, offset); */
  fs_close(fd);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
