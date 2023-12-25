#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* wuyc */
#include <sys/time.h>
#include <fcntl.h>
/* wuyc */
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

/* wuyc */
uint32_t NDL_GetTicks() {
  struct timeval tv;
  int ret = gettimeofday(&tv, NULL);
  int us;
  if (ret == 0)
  {
    us = tv.tv_sec * 1000000 + tv.tv_usec;
  }
  else
  {
    return 0;
  }
  return us;
  /* return 0; */
}

int NDL_PollEvent(char *buf, int len) {
  /* FILE *f_events = fopen("/dev/events", "r"); */
  /* len = fread(buf, len, 1, f_events); */
  /* fclose(f_events); */
  int fd = open("/dev/events", 0);
  len = read(fd, buf, len);
  close(fd);
  return len ? 1 : 0;
}
/* wuyc */
static inline void read_dispinfo(int *w, int *h)
{
  /* printf("hello\n"); */
  char buf[128];
  int fd = open("/proc/dispinfo", 0);
  read(fd, buf, 128);
  close(fd);
  /* printf("%s\n", buf); */
  char *tok = strtok(buf, ":");
  tok = strtok(NULL, "\n");
  /* printf("%s\n", tok); */
  *w = atoi(tok);
  tok = strtok(NULL, ":");
  tok = strtok(NULL, ":");
  /* printf("%s\n", tok); */
  *h = atoi(tok);
}

static int canvas_w, canvas_h;
void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  /* printf("%d %d", *w, *h); */
  if (*w == 0 && *h == 0)
  {
    read_dispinfo(&canvas_w, &canvas_h);
    *w = canvas_w;
    *h = canvas_h;
  }
  else
  {
    canvas_w = *w, canvas_h = *h;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int disp_w, disp_h;
  read_dispinfo(&disp_w, &disp_h);
  /* printf("%d %d\n", disp_w, disp_h); */
  int fd = open("/dev/fb", 0);
  int real_x = (disp_w - canvas_w) / 2 + x; 
  int real_y = (disp_h - canvas_h) / 2 + y;
  /* int real_x = 0, real_y = 0; */
  /* printf("%d, %d\n", real_x, real_y); */
  int offset = (disp_w * real_y + real_x) * sizeof(int);
  for (int i = 0; i < h; i++)
  {
    lseek(fd, offset, SEEK_SET);
    write(fd, pixels, w * sizeof(int));
    offset += disp_w * sizeof(int);
    pixels += w;
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
