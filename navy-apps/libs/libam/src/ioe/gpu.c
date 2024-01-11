#include <am.h>
/* wuyc */
#include <NDL.h>
/* wuyc */

/* wuyc */
void read_dispinfo(int *w, int *h);
/* wuyc */

void __am_gpu_init() {
  /* SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); */
  /* window = SDL_CreateWindow("Native Application", */
  /*     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, */
/* #ifdef MODE_800x600 */
  /*     W, H, */
/* #else */
  /*     W * 2, H * 2, */
/* #endif */
  /*     SDL_WINDOW_OPENGL); */
  /* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 32, */
  /*     RMASK, GMASK, BMASK, AMASK); */
  /* SDL_AddTimer(1000 / FPS, texture_sync, NULL); */
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int W, H;
  read_dispinfo(&W, &H);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = W, .height = H,
    .vmemsz = 0
  };
}

void __am_gpu_status(AM_GPU_STATUS_T *stat) {
  stat->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  NDL_DrawRect(ctl->pixels, ctl->x, ctl->y, ctl->w, ctl->h);
  /* printf("%d, %d, %d, %d\n", ctl->x, ctl->y, ctl->w, ctl->h); */
  /* int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h; */
  /* if (w == 0 || h == 0) return; */
  /* feclearexcept(-1); */
  /* SDL_Surface *s = SDL_CreateRGBSurfaceFrom(ctl->pixels, w, h, 32, w * sizeof(uint32_t), */
  /*     RMASK, GMASK, BMASK, AMASK); */
  /* SDL_Rect rect = { .x = x, .y = y }; */
  /* SDL_BlitSurface(s, NULL, surface, &rect); */
  /* SDL_FreeSurface(s); */
}
