#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  /* wuyc */
  /* w = 0; */
  /* h = 0; */
  /* wuyc */
  NDL_OpenCanvas(&w, &h);
  /* wuyc */
  printf("WIDTH: %d\nHEIGHT: %d\n", w, h);
  FILE *fp = fopen("/dev/fb", "w");
#define len 400
  uint32_t buf[len];
  for (int i = 0; i < len; i++)
    buf[i] = 0x000000FF;
  fwrite(buf, len, 1, fp);
  fclose(fp);
  /* wuyc */
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
