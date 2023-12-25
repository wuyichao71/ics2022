#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  /* void *bmp = BMP_Load("/share/slides/slides-0.bmp", &w, &h); */
  assert(bmp);
  /* wuyc */
  /* w = 0; */
  /* h = 0; */
  /* wuyc */
  NDL_OpenCanvas(&w, &h);
  /* wuyc */
  /* printf("WIDTH: %d\nHEIGHT: %d\n", w, h); */
  /* int fd = open("/dev/fb", 0, 0); */
  /* lseek(fd, 400, SEEK_SET); */
/* #define len 40000 */
  /* uint32_t buf[len]; */
  /* for (int i = 0; i < len; i++) */
  /*   buf[i] = 0x000000FF; */
  /* write(fd, buf, len); */
  /* close(fd); */
  /* wuyc */
  NDL_DrawRect(bmp, 0, 0, w, h);
  w = 0;
  h = 0;
  bmp = BMP_Load("/share/slides/slides-0.bmp", &w, &h);
  printf("I am here2\n");
  free(bmp);
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
