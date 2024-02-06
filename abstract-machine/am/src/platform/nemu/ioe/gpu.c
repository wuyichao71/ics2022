#include <am.h>
#include <nemu.h>
/* wuyc */
#include <klib.h>
/* wuyc */

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
  /* int x, y; */
  int y;
  int fb_offset = ctl->y * cfg.width + ctl->x;
  int pixels_offset = 0;
  uint32_t *pixels = (uint32_t *)ctl->pixels;
  int w_byte = sizeof(uint32_t) * ctl->w;
  for (y = 0; y < ctl->h; y++)
  {
    memcpy(fb + fb_offset, pixels + pixels_offset, w_byte);
    /* for (x = 0; x < ctl->w; x++) */
    /* { */
    /*   uint32_t in_pixels = pixels[pixels_offset + x]; */
    /*   /1* uint32_t out_pixels = fb[fb_offset + x]; *1/ */
    /*   /1* if (out_pixels != in_pixels) *1/ */
    /*   /1* *(fb + fb_offset + x) = *(pixels + pixels_offset + x); *1/ */
    /*   fb[fb_offset + x] = in_pixels; */

    /* } */
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
