#include <stdio.h>
#include <NDL.h>
/* #include <sys/time.h> */

int main()
{
  int sec = 1;
  int us;
  /* struct timeval tv; */
  
  NDL_Init(0);
  while (1)
  {
    do
    {
      /* gettimeofday(&tv, NULL); */
      /* us = tv.tv_sec * 1000000 + tv.tv_usec; */
      us = NDL_GetTicks();
    } while (us / 500 < sec);
    printf("Hello World at %d microsecond\n", us);
    sec += 1;
  }
  NDL_Quit();
  return 0;
}
