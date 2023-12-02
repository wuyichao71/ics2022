#include <stdio.h>
#include <sys/time.h>

int main()
{
  float sec = 0.5;
  float us;
  struct timeval tv;
  
  while (1)
  {
    do
    {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      us = tv.tv_sec + tv.tv_usec / 1000000;
    } while (us < sec);
    printf("Hello World at %d second %d microsecond\n", tv.tv_sec, tv.tv_usec);
    sec += 0.5;
  }
}
