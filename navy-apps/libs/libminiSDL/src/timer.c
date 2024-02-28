#include <NDL.h>
/* #include <sdl-timer.h> */
#include <stdio.h>
/* wuyc */
#include <SDL.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
/* #define MAX_TIMER 16 */
/* static SDL_Timer timer[MAX_TIMER] = {}; */
/* static int previous_time[MAX_TIMER] = {}; */
static SDL_Timer SDL_Timer_Begin = {};
static SDL_Timer SDL_Timer_End = {};
/* static SDL_Timer *SDL_Timer_Header = &SDL_Timer_Node; */
extern uint32_t g_flags;
static bool incall = false;
void CallbackHelper();
/* wuyc */

void SDL_Init_Timer()
{
  SDL_Timer_Begin.next = &SDL_Timer_End;
  SDL_Timer_End.prev = &SDL_Timer_Begin;
}

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  SDL_Timer *p = (SDL_Timer *)malloc(sizeof(SDL_Timer));
  if (p != NULL)
  {
    p->next = SDL_Timer_Begin.next;
    p->prev = &SDL_Timer_Begin;
    SDL_Timer_Begin.next = p;
    p->next->prev = p;
    return p;
  }
  return NULL;
  /* assert(0); */
  /* for (int i = 0; i < MAX_TIMER; i++) */
  /* { */
  /*   if (!timer[i].used) */
  /*   { */
  /*     timer[i] = (SDL_Timer){.used = 1, .interval = interval, .callback = callback, .param = param}; */
  /*     previous_time[i] = SDL_GetTicks(); */
  /*     return &timer[i]; */
  /*   } */
  /* } */
}

int SDL_RemoveTimer(SDL_TimerID id) {
  /* assert(0); */
  /* ((SDL_Timer *)id)->used = 0; */
  SDL_Timer *p = (SDL_Timer *)id;
  p->next->prev = p->prev;
  p->prev->next = p->next;
  free(id);
  return SDL_TRUE;
}

void TimerCallbackHelper()
{
  if (incall)
    return;
  else
  {
    incall = true;
    SDL_Timer *p = SDL_Timer_Begin.next;
    for (; p != &SDL_Timer_End; p = p->next)
    {
      int current_time = SDL_GetTicks();
      if (p->interval == 0)
        p->previous_time = current_time;
      else if (current_time >= p->previous_time + p->interval)
      {
        p->interval = p->callback(p->interval, p->param);
        p->previous_time = current_time;
      }
    }
    /* for (int i = 0; i < MAX_TIMER; i++) */
    /* { */
    /*   if (timer[i].used) */
    /*   { */
    /*     int current_time = SDL_GetTicks(); */
    /*     if (timer[i].interval == 0) */
    /*       previous_time[i] = current_time; */
    /*     else if (current_time >= previous_time[i] + timer[i].interval) */
    /*     { */
    /*       timer[i].interval = timer[i].callback(timer[i].interval, timer[i].param); */
    /*       previous_time[i] = current_time; */
    /*     } */
    /*   } */
    /* } */
    incall = false;
  }
}

uint32_t SDL_GetTicks() {
  /* return 0; */
  CallbackHelper();
  if (g_flags & SDL_INIT_TIMER) TimerCallbackHelper();
  return NDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
}
