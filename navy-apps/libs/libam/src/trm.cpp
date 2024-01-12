#include <am.h>
/* wuyc */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
/* #include <stdio.h> */
/* wuyc */
Area heap;

void putch(char ch) {
  /* write(1, &ch, 1); */
  putchar(ch);
}

void halt(int code) {
  printf("Exit code = %02xh\n", code);
  /* putch('&'); */
  exit(code);
}
