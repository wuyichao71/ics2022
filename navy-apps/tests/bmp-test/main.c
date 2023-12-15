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
  uint32_t buf[50000] = {0x00FF0000};
  fwrite(buf, 50000, 1, fp);
  fclose(fp);
  /* wuyc */
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
