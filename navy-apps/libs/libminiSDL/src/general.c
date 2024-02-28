#include <NDL.h>
/* wuyc */
/* #include <stdio.h> */
#include <SDL.h>
void SDL_Init_Timer();
uint32_t g_flags = 0;
/* wuyc */

int SDL_Init(uint32_t flags) {
  /* printf("In SDL_Init\n"); */
  if (flags & SDL_INIT_TIMER)
  {
    SDL_Init_Timer();
  }
  g_flags = flags;
  return NDL_Init(flags);
}

void SDL_Quit() {
  NDL_Quit();
}

char *SDL_GetError() {
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  return -1;
}

int SDL_ShowCursor(int toggle) {
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}
