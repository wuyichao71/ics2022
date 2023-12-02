#include <stdio.h>
#include <sys/time.h>

int main()
{
  int sec = 1;
  int us;
  struct timeval tv;
  
  while (1)
  {
    do
    {
      gettimeofday(&tv, NULL);
      us = tv.tv_sec * 1000000 + tv.tv_usec;
    } while (us / 500000 < sec);
    printf("Hello World at %d second %d microsecond\n", tv.tv_sec, tv.tv_usec);
    sec += 1;
  }
}
