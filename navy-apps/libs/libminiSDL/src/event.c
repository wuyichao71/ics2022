#include <NDL.h>
#include <SDL.h>
/* wuyc */
#include <assert.h>
#include <string.h>
void CallbackHelper();
void TimerCallbackHelper();
extern uint32_t g_flags;
/* wuyc */

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

/* wuyc */
#define N_KEY (sizeof(keyname) / sizeof(keyname[0]))
static uint8_t key_snapshot[N_KEY] = {0};

// queue
#define MAX_EVENTS 128
#define NEXT(index) ((index + 1) % MAX_EVENTS)
static SDL_Event event_queue[MAX_EVENTS];
static int first = 0, last = -1;

inline static int event_count()
{
  if (last == -1)
    return 0;
  else
    return (last + MAX_EVENTS - 1 - first) % MAX_EVENTS + 1;
}

inline static int event_push(SDL_Event *ev)
{
  int count = event_count();
  if (count == 0)
  {
    event_queue[first] = *ev;
    last = NEXT(first);
  }
  else if (count == MAX_EVENTS)
  {
    return -1;
  }
  else
  {
    event_queue[last] = *ev;
    last = NEXT(last);
  }
  return 1;
}

inline static int event_pop(SDL_Event *ev)
{
  int count = event_count();
  if (count == 0)
  {
    return -1;
  }
  else if (count == 1)
  {
    *ev = event_queue[first];
    first = NEXT(first);
    last = -1;
  }
  else
  {
    *ev = event_queue[first];
    first = NEXT(first);
  }
  return 1;
}

int SDL_PushEvent(SDL_Event *ev) {
  return event_push(ev);
  /* assert(0); */
  /* return 0; */
}

int SDL_PollEvent(SDL_Event *ev) {
  /* wuyc */
  /* if (1) */
  /* { */
    char buf[64];
    int ret = 0;
    ret = NDL_PollEvent(buf, sizeof(buf));
    /* printf("%s", buf); */
    if (ret == 1)
    {
      char *type_str = strtok(buf, " ");
      if (type_str[1] == 'd')
        ev->type = SDL_KEYDOWN;
      else if (type_str[1] == 'u')
        ev->type = SDL_KEYUP;
      /* printf("%s", buf); */
      char *kd_str = strtok(NULL, "\n");
      for (int i = 0; i < N_KEY; i++)
      {
        if (strcmp(kd_str, keyname[i]) == 0)
        {
          ev->key.keysym.sym = i;
          key_snapshot[i] = (ev->type == SDL_KEYDOWN) ? 1 : 0;
          return 1;
        }
          /* printf("keyname = %s\n", keyname[i]); */
      }
    }
    /* else */
    /* { */
      /* ev->type = SDL_KEYUP; */
      /* ev->key.keysym.sym = SDLK_NONE; */
      /* return 0; */
    /* } */
    /* printf("%d\n", sizeof(keyname)); */
    /* printf("%d\n", sizeof(keyname[1])); */
    /* printf("%d\n", sizeof((char *)buf)); */
    /* printf("%s_\n", kd_str); */
  /* } */
  /* else */
  /* { */
  /*   return event_pop(ev); */
  /* } */
  /* printf("ret = %d\n", ret); */
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  /* wuyc */
  /* CallbackHelper(); */
  /* if (g_flags & SDL_INIT_TIMER) TimerCallbackHelper(); */
  /* if (1) */
  /* { */
    char buf[64];
    int ret = 0;
    while (ret == 0)
    {
      ret = NDL_PollEvent(buf, sizeof(buf));
      /* printf("ret = %d\n", ret); */
      if (ret == 1)
      {
        char *type_str = strtok(buf, " ");
        if (type_str[1] == 'd')
          event->type = SDL_KEYDOWN;
        else if (type_str[1] == 'u')
          event->type = SDL_KEYUP;
        char *kd_str = strtok(NULL, "\n");
        /* printf("%s %s\n", buf, kd_str); */
        for (int i = 0; i < N_KEY; i++)
        {
          if (strcmp(kd_str, keyname[i]) == 0)
          {
            event->key.keysym.sym = i;
            key_snapshot[i] = (event->type == SDL_KEYDOWN) ? 1 : 0;
            return 1;
          }
            /* printf("keyname = %s\n", keyname[i]); */
        }
        /* printf("%d\n", sizeof(keyname)); */
        /* printf("%d\n", sizeof(keyname[1])); */
        /* printf("%d\n", sizeof((char *)buf)); */
        /* printf("%s_\n", kd_str); */
      }
      /* else */
      /* { */
        /* event->type = SDL_KEYUP; */
        /* event->key.keysym.sym = SDLK_NONE; */
        /* return 1; */
      /* } */
    }
  /* } */
  /* else */
  /* { */
  /*   return event_pop(event); */
  /* } */
  /* printf("ret = %d\n", ret); */
  return 0;
  /* wuyc */
}


int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  /* assert(0); */
  int i = 0;
  if (action == SDL_GETEVENT)
  {
    if (event_count() == 0)
      return 0;
    else
    {
      int j = first;
      for (int k = first; k != last; k = NEXT(k))
      {
        if (i < numevents && SDL_EVENTMASK(event_queue[k].type) == mask)
        {
          *(ev + i) = event_queue[k];
          i++;
        }
        else
        {
          event_queue[j] = event_queue[k];
          j = NEXT(j);
        }
      }
      last = j;
    }
    /* for (int i = 0; i < numevents; i++) */
    /* { */
    /*   int error = event_pop(ev + i); */
    /*   if (error != 0) return error; */
    /* } */
  }
  return i;
  /* return 0; */
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  /* while (1); */
  /* printf("N_KEY = %d\n", N_KEY); */
  if (numkeys)
    *numkeys = N_KEY;

   return key_snapshot;
  /* return NULL; */
}
