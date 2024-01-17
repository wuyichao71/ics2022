/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
/* wuyc */
static volatile int count = 0;

static void audio_play(void *userdata, uint8_t *stream, int len) {
  int i;
  for (i = 0; i < len; i++)
    stream[i] = 0;
  count = 0;
  /* if (len >= count) */
  /* { */
  /*   for (i = 0; i < count; i++) */
  /*     stream[i] = sbuf[i]; */
  /*   memset(stream + i, 0, len - count); */
  /*   /1* for(; i < len; i++) *1/ */
  /*     /1* stream[i] = 0; *1/ */
  /*   count = 0; */
  /* } */
  /* else */
  /* { */
  /*   for (i = 0; i < len; i++) */
  /*     stream[i] = sbuf[i]; */
  /*   for (; i < count; i++) */
  /*     sbuf[i - len] = sbuf[i]; */
  /*   count -= len; */
  /* } */
}
/* wuyc */

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  /* if (!is_write && offset == reg_sbuf_size * sizeof(uint32_t)) */
    /* audio_base[reg_sbuf_size] = CONFIG_SB_SIZE; */
  if (!is_write && offset == reg_count * sizeof(uint32_t))
    audio_base[reg_count] = count;
  else if (is_write && offset == reg_init * sizeof(uint32_t))
  {
    if (audio_base[reg_init])
    {
      SDL_AudioSpec s = {};
      s.format = AUDIO_S16SYS;
      s.freq = audio_base[reg_freq];
      s.channels = audio_base[reg_channels];
      s.samples = audio_base[reg_samples];
      s.callback = audio_play;
      s.userdata = NULL;

      count = 0;
      int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
      if (ret == 0)
      {
        SDL_OpenAudio(&s, NULL);
        SDL_PauseAudio(0);
        /* audio_base[reg_init] = 0; */
      } 
    } 
  }
}

static void audio_sbuf_handler(uint32_t offset, int len, bool is_write) {
  if (is_write)
    count++;
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
  /* add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, NULL); */
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  /* wuyc */
  /* add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL); */
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, audio_sbuf_handler);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
  /* wuyc */
}
