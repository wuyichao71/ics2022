#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  /* int i; */
  /* AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG); */
  /* int w = cfg.width; */
  /* int h = cfg.height; */
  /* uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR; */
  /* for (i = 0; i < w * h; i ++) fb[i] = i; */
  /* outl(SYNC_ADDR, 1); */
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int32_t data = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = data >> 16, .height = data & 0x0000ffff,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  int x, y;
  int fb_offset = ctl->y * cfg.width + ctl->x;
  int pixels_offset = 0;
  uint32_t *pixels = (uint32_t *)ctl->pixels;
  for (y = 0; y < ctl->h; y++)
  {
    for (x = 0; x < ctl->w; x++)
    {
      /* ifn(fb[fb_offset + x] != ((uint32))) */
      *(fb + fb_offset + x) = *(pixels + pixels_offset + x);

    }
    fb_offset += cfg.width;
    pixels_offset += ctl->w;
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
