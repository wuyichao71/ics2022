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
static SDL_AudioSpec current_spec;
/* static int length = 0; */
/* typedef void (*callback_t)(void *userdata, Uint8 *stream, int len); */
/* static callback_t callback = NULL; */
static bool incall = false;
/* wuyc */

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  desired->size = desired->samples * desired->channels * desired->format / 8;
  current_spec = *desired;
  if (obtained != NULL)
    *obtained = *desired;
  /* printf("%d, %d, %d\n", desired->freq, desired->channels, desired->samples); */
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  /* printf("In SDL_OpenAudio\n"); */
  /* time_interval = 1000 * desired->samples / desired->freq / 2; */
  time_interval = 1000 * desired->samples / desired->freq;
  /* samples = desired->samples; */
  /* printf("%d\n", desired->samples); */
  stream = (uint8_t *)malloc(desired->size);
  /* printf("size = %d\n", desired->size); */
  /* printf("length = %d\n", length); */
  /* callback = desired->callback; */
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
    previous_time = SDL_GetTicks();
    int rest = NDL_QueryAudio();
    /* printf("Before: %d, %d\n", rest, length); */
    rest = current_spec.size <= rest ? current_spec.size : rest;
    /* printf("After: %d, %d\n", rest, length); */
    current_spec.callback(NULL, stream, rest);
    NDL_PlayAudio(stream, rest);
  }
  else
    silence = true;
}

void CallbackHelper()
{
  if (incall)
    return;
  else
  {
    incall = true;
    bool not_run = false;
    if (silence)
      not_run = true;

    int current_time = SDL_GetTicks();
    if (current_time < previous_time + time_interval)
      not_run = true;

    if (not_run == false)
    {
      int rest = NDL_QueryAudio();
      /* printf("Before: %d, %d\n", rest, length); */
      rest = current_spec.size <= rest ? current_spec.size : rest;
      /* printf("After: %d, %d\n", rest, length); */
      current_spec.callback(NULL, stream, rest);
      NDL_PlayAudio(stream, rest);
      previous_time = current_time;
    }
    incall = false;
    return;
  }
}

#define MIXAUDIO(type, min_value, max_value) { \
  int value; \
  int length = len / (sizeof(type)); \
  for (int i = 0; i < length; i++) \
  { \
    value = ((type *)src)[i] * volume / SDL_MIX_MAXVOLUME; \
    value += ((type *)dst)[i]; \
    if (value < (min_value)) value = (min_value); \
    if (value > (max_value)) value = (max_value); \
    ((type *)dst)[i] = value; \
  } \
} while (0)

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  switch (current_spec.format)
  {
    case AUDIO_S16SYS:
      MIXAUDIO(Sint16, -(1<<15), 1<<15-1);
      break;
    case AUDIO_U8:
      MIXAUDIO(Sint16, 0, 1<<8-1);
      break;
    default:
      /* panic("UNKNOWN FORMAT"); */
      assert(0);
      break;
  }
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  FILE *fp = fopen(file, "r");
  uint32_t ChunkID, ChunkSize, Format, Subchunk1ID, 
           Subchunk1Size, SampleRate, ByteRate, 
           Subchunk2ID, Subchunk2Size;
  uint16_t AudioFormat, NumChannels, BlockAlign, BitsPerSample;
  uint16_t ExtraParamSize;
  uint8_t *ExtraParams = NULL;

  fread(&ChunkID, sizeof(ChunkID), 1, fp);
  assert(ChunkID == 0x46464952);
  fread(&ChunkSize, sizeof(ChunkSize), 1, fp);
  /* printf("%d\n", ChunkSize); */
  fread(&Format, sizeof(Format), 1, fp);
  /* printf("0x%04x\n", Format); */
  assert(Format == 0x45564157);
  fread(&Subchunk1ID, sizeof(Subchunk1ID), 1, fp);
  /* printf("0x%04x\n", Subchunk1ID); */
  assert(Subchunk1ID == 0x20746d66);
  fread(&Subchunk1Size, sizeof(Subchunk1Size), 1, fp);
  /* printf("%d\n", Subchunk1Size); */
  assert(Subchunk1Size == 16);
  fread(&AudioFormat, sizeof(AudioFormat), 1, fp);
  /* printf("%d\n", AudioFormat); */
  assert(AudioFormat == 1);
  fread(&NumChannels, sizeof(NumChannels), 1, fp);
  /* printf("%d\n", NumChannels); */
  fread(&SampleRate, sizeof(SampleRate), 1, fp);
  /* printf("%d\n", SampleRate); */
  fread(&ByteRate, sizeof(ByteRate), 1, fp);
  /* printf("%d\n", ByteRate); */
  fread(&BlockAlign, sizeof(BlockAlign), 1, fp);
  /* printf("%d\n", BlockAlign); */
  fread(&BitsPerSample, sizeof(BitsPerSample), 1, fp);
  /* printf("%d\n", BitsPerSample); */
  if (Subchunk1Size != 16)
  {
    fread(&ExtraParamSize, sizeof(ExtraParamSize), 1, fp);
    ExtraParams = (uint8_t *)malloc(ExtraParamSize);
    free(ExtraParams);
    fread(ExtraParams, ExtraParamSize, 1, fp);
  }
  fread(&Subchunk2ID, sizeof(Subchunk2ID), 1,fp);
  /* printf("0x%04x\n", Subchunk2ID); */
  assert(Subchunk2ID == 0x61746164);
  fread(&Subchunk2Size, sizeof(Subchunk2Size), 1,fp);
  /* printf("%d\n", Subchunk2Size); */
  *audio_buf = (uint8_t *)malloc(Subchunk2Size);
  fread(*audio_buf, Subchunk2Size, 1, fp);
  *audio_len = Subchunk2Size;

  spec->freq = SampleRate;
  spec->channels = NumChannels;
  spec->format = BitsPerSample;
  spec->userdata = NULL;
  /* printf("%d\n", spec->format); */

  fclose(fp);
  /* exit(1); */
  /* return NULL; */
  return spec;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  if (audio_buf)
    free(audio_buf);
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
