#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

typedef struct
{
  uint8_t special:1;
  uint8_t left:1;
  uint8_t plus:1;
  uint8_t space:1;
  uint8_t zeropad:1;
} Flags;

/* int printf(const char *fmt, ...) { */
/*   panic("Not implemented"); */
/* } */

/* static char *_out; */
/* static void (*_putch)(char); */
/* _putch = putch; */

static inline int fmt_atoi(const char **fmt, int initial)
{
  int num = initial;
  if (**fmt >= '0' && **fmt <= '9')
    num = atoi(*fmt);
  for (; **fmt >= '0' && **fmt <= '9'; (*fmt)++);
  return num;
}


static inline Flags fmt_flags(const char **fmt)
{
  Flags flags = {0};
  int flags_label = 1;
  while (flags_label)
  {
    switch (**fmt)
    {
      case '#': flags.special = 1;    break;
      case '0': flags.zeropad = 1;    break;
      case '-': flags.left = 1;       break;
      case ' ': flags.space = 1;      break;
      case '+': flags.plus = 1;       break;
      default:  flags_label = 0;      continue;
    }
    (*fmt)++;
  }
  return flags;
}

static inline int fmt_field_width(const char **fmt)
{
  return fmt_atoi(fmt, 0);
}

static inline int fmt_precision(const char **fmt)
{
  int precision = -1;
  if (**fmt == '.')
  {
    (*fmt)++;
    precision = fmt_atoi(fmt, -1);
  }
  return precision;
}

static inline int fmt_length_modifier(const char **fmt)
{
  if (**fmt == 'h' && *(*fmt + 1) == 'h')
  {
    (*fmt) += 2;
    return 's';
  }
  else if (**fmt == 'l' && *(*fmt + 1) == 'l')
  {
    (*fmt) += 2;
    return 'q';
  }
  else if (**fmt == 'h' || **fmt == 'l')
  {
    char c = **fmt;
    (*fmt) += 1;
    return c;
  }

  return -1;
}

static inline int fmt_output_string(char *s, char *out, int slen, int field_width, int precision, Flags flags)
{
  int len = strlen(s);
  if (precision >= 0)
    len = precision > len? len : precision;
  if (!flags.left)
  {
    while (field_width > len)
    {
      *(out + slen) = ' ';
      slen++;
      field_width--;
    }
  }
  for (int i = 0; i < len; i++)
  {
    *(out + slen) = *s;
    slen++;
    s++;
  }
  while (field_width > len)
  {
      *(out + slen) = ' ';
      slen++;
      field_width--;
  }
  return slen;
}

static inline int fmt_output_number(uint64_t num, char *out, int slen, int base, int field_width, int precision, Flags flags)
/* static inline int fmt_output_number(int num, char *out, int slen, int base, int field_width, int precision, Flags flags) */
{
  char sign = 0;
  char tmp_str[100];
  char char_pre = ' ';
  int num_len = 0;
  int rem;

  if (precision < 0 && flags.zeropad)
    char_pre = '0';

  if ((signed long long int)num < 0)
  {
    sign = '-';
    num = -num;
  }
  else if (flags.plus)
    sign = '+';
  else if (flags.space)
    sign = ' ';

  do
  {
    rem = num % base;
    num /= base;
    tmp_str[num_len] = rem + '0';
    num_len++;
  } while (num > 0);

  if (num_len > precision)
    precision = num_len;

  field_width -= precision;

  if (sign)
    field_width--;

  if (!flags.left)
  {
    while (field_width > 0)
    {
      *(out + slen) = char_pre;
      slen++;
      field_width--;
    }
  }

  if (sign)
  {
    *(out + slen) = sign;
    slen++;
  }

  while (precision > num_len)
  {
      *(out + slen) = '0';
      slen++;
      precision--;
  }

  for (int i = num_len; i > 0; i--)
  {
    *(out + slen) = tmp_str[i - 1];
    slen++;
  }

  while (field_width > 0)
  {
    *(out + slen) = ' ';
    slen++;
    field_width--;
  }

  return slen;
}


int vsprintf(char *out, const char *fmt, va_list ap) {
  int slen = 0;
  for(; *fmt; fmt++)
  {
    if(*fmt == '%')
    {
      const char *fmt_org = fmt;
      fmt++;

      // Flag characters
      Flags flags = fmt_flags(&fmt);

      // Field width
      int field_width = fmt_field_width(&fmt);

      // Precision
      int precision = fmt_precision(&fmt);

      // Length modifier
      int length_modifier = fmt_length_modifier(&fmt);

      // va_list
      // For %s
      char *s;

      // For %d
      int is_integer = 0;
      uint64_t num;
      /* int num; */
      int base = 10;

      // For %f
      int is_float = 0;

      switch(*fmt)
      {
        // %s(string)
        case 's':
          s = va_arg(ap, char *);
          slen = fmt_output_string(s, out, slen, field_width, precision, flags);
          continue;

        // %d(number)
        case 'd':
          is_integer = 1;
          break;
        // no match
        default:
          for(const char *str = fmt_org; str <= fmt; str++)
          {
            *(out + slen) = *str;
            slen++;
          }
          continue;
      }
      /* slen = number(out, slen, num, base, flags); */
      if (is_integer)
      {
        switch (length_modifier)
        {
          case 's':
            num = (unsigned char)va_arg(ap, int);
            num = (signed char)num;
            break;
          case 'h':
            num = (unsigned short int)va_arg(ap, int);
            num = (signed short)num;
            break;
          case 'l':
            num = va_arg(ap, uint32_t);
            /* printf("%ld\n", sizeof(unsigned long int)); */
            /* num = (signed long int)num; */
            break;
          case 'q':
            num = va_arg(ap, uint64_t);
            /* printf("%ld\n", sizeof(long long int)); */
            /* num = (signed long long int)num; */
            break;
          default:
            num = va_arg(ap, int);
            break;
        }
        slen = fmt_output_number(num, out, slen, base, field_width, precision, flags);
      }
      else if (is_float)
      {
      }
    }
    else
    {
      *(out + slen) = *fmt;
      slen++;
    }
  }
  *(out + slen) = '\0';
  return slen;
  /* panic("Not implemented"); */
}

int sprintf(char *out, const char *fmt, ...) {
  int slen;
  va_list ap;
  va_start(ap,fmt);
  slen = vsprintf(out, fmt, ap);
  va_end(ap);
  return slen;
  /* panic("Not implemented"); */
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
