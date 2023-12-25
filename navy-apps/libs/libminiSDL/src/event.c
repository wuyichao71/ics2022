#include <NDL.h>
#include <SDL.h>
/* wuyc */
#include <string.h>
/* wuyc */

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  /* wuyc */
  char buf[64];
  int ret = 0;
  while (ret == 0)
  {
    ret = NDL_PollEvent(buf, sizeof(buf));
    if (ret == 1)
    {
      char *type_str = strtok(buf, " ");
      if (type_str[1] == 'd')
        event->type = SDL_KEYDOWN;
      else if (type_str[1] == 'u')
        event->type = SDL_KEYUP;
      /* printf("%s", buf); */
      char *kd_str = strtok(NULL, "\n");
      for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++)
      {
        if (strcmp(kd_str, keyname[i]) == 0)
        {
          event->key.keysym.sym = i;
          return 1;
        }
          /* printf("keyname = %s\n", keyname[i]); */
      }
      /* printf("%d\n", sizeof(keyname)); */
      /* printf("%d\n", sizeof(keyname[1])); */
      /* printf("%d\n", sizeof((char *)buf)); */
      /* printf("%s_\n", kd_str); */
    }
  }
  /* printf("ret = %d\n", ret); */
  return 0;
  /* wuyc */
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
