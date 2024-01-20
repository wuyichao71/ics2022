#include <NDL.h>
#include <SDL.h>
/* wuyc */
#include <stdio.h>
/* wuyc */

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  if (obtained != NULL)
    *obtained = *desired;
  /* printf("%d, %d, %d\n", desired->freq, desired->channels, desired->samples); */
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  /* printf("In SDL_OpenAudio\n"); */
  return 0;
}

void SDL_CloseAudio() {
  NDL_CloseAudio();
}

void SDL_PauseAudio(int pause_on) {
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
