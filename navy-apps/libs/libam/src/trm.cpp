#include <am.h>
/* wuyc */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <klib-macros.h>
/* #include <stdio.h> */
/* wuyc */
Area heap;
/* wuyc */
/* #define PMEM_SIZE (128 * 1024 * 1024) */
/* char pmem[PMEM_SIZE] = {}; */
/* Area heap = RANGE(pmem, pmem+PMEM_SIZE); */
/* wuyc */

void putch(char ch) {
  /* write(1, &ch, 1); */
  putchar(ch);
}

void halt(int code) {
  printf("Exit code = %02xh\n", code);
  /* putch('&'); */
  exit(code);
}
