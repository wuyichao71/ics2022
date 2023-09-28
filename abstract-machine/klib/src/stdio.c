#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  int out_i = 0;
  int val_num = 0;
  va_list ap;
  va_start(ap,fmt);
  for(; *fmt; fmt++)
  {
    if(*fmt == '%')
    {
      val_num++;
      switch(*(++fmt))
      {
        case 's':
          break;

        case 'd':
          break;
        default:
          break;
      }

    }
    else
      out[out_i++] = *fmt;
  }
  return out_i;
  /* panic("Not implemented"); */
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
