#include <stdio.h>
#include <fixedptc.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

void compare(int a, int b, fixedpt i, double f)
{
  if (a != b)
    printf("i = %d, f = %lf, a = %d, b = %d\n", i, f, a, b);
  assert(a == b);
}

int main()
{
  double f;
  volatile int a, b;
  /* Check fixedpt */
  fixedpt i;
  i = INT_MIN;
  while(1)
  {
    f = (double)i / (double)FIXEDPT_ONE;
    a = (int)ceil(f) << FIXEDPT_FBITS;
    b = fixedpt_ceil(i);
    compare(a, b, i, f);
    if (i == INT_MAX - FIXEDPT_ONE)
      break;
    i++;
  }
  printf("ceil is OK\n");

  i = INT_MIN;
  while(1)
  {
    f = (double)i / (double)FIXEDPT_ONE;
    a = (int)floor(f) << FIXEDPT_FBITS;
    b = fixedpt_floor(i);
    compare(a, b, i, f);
    if (i == INT_MAX)
      break;
    i++;
  }
  printf("floor is OK\n");

  i = INT_MIN;
  while(1)
  {
    f = (double)i / (double)FIXEDPT_ONE;
    a = floor(f);
    b = fixedpt_toint(i);
    compare(a, b, i, f);
    if (i == INT_MAX)
      break;
    i++;
  }
  printf("fixedpt is OK\n");

}
