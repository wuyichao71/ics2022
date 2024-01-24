#include <NDL.h>
#include <SDL.h>
/* wuyc */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

static int time_interval = 0; // ms
static bool silence = true;
static int previous_time = 0;
static uint8_t *stream = NULL;
/* static int samples = 0; */
static int length = 0;
typedef void (*callback_t)(void *userdata, Uint8 *stream, int len);
static callback_t callback = NULL;
/* wuyc */

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  if (obtained != NULL)
    *obtained = *desired;
  /* printf("%d, %d, %d\n", desired->freq, desired->channels, desired->samples); */
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  /* printf("In SDL_OpenAudio\n"); */
  time_interval = 1000 * desired->samples / desired->freq;
  /* samples = desired->samples; */
  length = desired->samples * desired->channels * desired->format / 8;
  stream = (uint8_t *)malloc(length);
  callback = desired->callback;
  /* printf("time_interval = %d\n", time_interval); */
  return 0;
}

void SDL_CloseAudio() {
  NDL_CloseAudio();
  free(stream);
}

void SDL_PauseAudio(int pause_on) {
  assert(pause_on == 0 || pause_on == 1);
  if (pause_on == 0)
  {
    silence = false;
    previous_time = NDL_GetTicks();
    int rest = NDL_QueryAudio();
    rest = length <= rest ? length : rest;
    callback(NULL, stream, rest);
    NDL_PlayAudio(stream, rest);
  }
  else
    silence = true;
}

void CallbackHelper()
{
  if (silence)
    return;

  int current_time = NDL_GetTicks();
  if (current_time < previous_time + time_interval)
    return;

  int rest = NDL_QueryAudio();
  rest = length <= rest ? length : rest;
  callback(NULL, stream, length);
  NDL_PlayAudio(stream, length);
  previous_time = current_time;
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
