#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *out_org = out;
  for(; *fmt; fmt++)
  {
    if(*fmt == '%')
    {
      const char *fmt_org = fmt;
      fmt++;

      // Flag characters
      int flag_label = 1;
      int special = 0, left = 0, plus = 0, space = 0, zeropad = 0;
      while (flag_label)
      {
        switch (*fmt)
        {
          case '#': special = 1;    break;
          case '0': zeropad = 1;    break;
          case '-': left = 1;       break;
          case ' ': space = 1;      break;
          case '+': plus = 1;       break;
          default:  flag_label = 0; break;
        }
      }
      *(&special) = special;
      *(&zeropad) = zeropad;
      *(&left) = left;
      *(&space) = space;
      *(&plus) = plus;
      

      // va_list
      char *s;
      int d;

      int num_len = 0;
      int div, rem;

      switch(*fmt)
      {
        // %s(string)
        case 's':
          s = va_arg(ap, char *);
          for(; *s; s++) 
          {
            *out = *s;
            out++;
          }
          continue;
        // %d(number)
        case 'd':
          d = va_arg(ap, int);
          div = d;
          do
          {
            num_len++;
            div = div / 10;
          } while(div != 0);
          /* move org */
          out += num_len;
          *out = '\0';
          for(int i = 1; i <= num_len; i++)
          {
            rem = d % 10;
            d = d / 10;
            *(out - i) = rem + '0';
          }
          break;
        // no match
        default:
          for(const char *str = fmt_org; str <= fmt; str++)
          {
            *out = *str;
            out++;
          }
          continue;
      }
    }
    else
      *out++ = *fmt;
  }
  *out = '\0';
  return out - out_org;
  /* panic("Not implemented"); */
}

int sprintf(char *out, const char *fmt, ...) {
  char *out_org = out;
  va_list ap;
  va_start(ap,fmt);
  vsprintf(out, fmt, ap);
  va_end(ap);
  return out - out_org;
  /* panic("Not implemented"); */
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
