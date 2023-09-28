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
  /* int va_num = 0; */
  va_list ap;
  /* int d; */
  char *s;
  va_start(ap,fmt);
  for(; *fmt; fmt++)
  {
    if(*fmt == '%')
    {
      switch(*(++fmt))
      {
        case 's':
          s = va_arg(ap, char *);
          for(; *s; s++)
          {
            *(out+out_i) = *s;
            out_i++;
          }
          *out = '\0';
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
  va_end(ap);
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
