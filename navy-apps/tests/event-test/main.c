#include <stdio.h>
#include <NDL.h>

int main() {
  NDL_Init(0);
  /* printf("Hello\n"); */
  while (1) {
    char buf[64];
    /* printf("%d\n", NDL_PollEvent(buf, sizeof(buf))); */
    if (NDL_PollEvent(buf, sizeof(buf))) {
      printf("receive event: %s\n", buf);
    }
  }
  return 0;
}
