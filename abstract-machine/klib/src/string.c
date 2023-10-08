#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i = 0;
  for(; s[i] != '\0'; i++);
  return i;
  /* panic("Not implemented"); */
}

char *strcpy(char *dst, const char *src) {
  for(size_t i= 0; (dst[i] = src[i]) != '\0'; i++);
  return dst;
  /* panic("Not implemented"); */
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for(i = 0; (dst[dst_len+i] = src[i]) != '\0'; i++);
  return dst;
  /* panic("Not implemented"); */
}

int strcmp(const char *s1, const char *s2) {
  int dif;
  for(size_t i = 0; (dif = s1[i] - s2[i]) == 0 && s1[i] != '\0' && s2[i] != '\0'; i++);
  return dif;
  /* panic("Not implemented"); */
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  for(size_t i = 0; i < n; i++) *(unsigned char *)(s+i) = c;
  return s;
  /* panic("Not implemented"); */
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  for(size_t i = 0; i < n; i++)
    *(unsigned char *)(out + i) = *(unsigned char *)(in + i);
  return out;
  /* panic("Not implemented"); */
}

int memcmp(const void *s1, const void *s2, size_t n) {
  int dif = 0;
  for(size_t i = 0; i < n; i++)
  {
    dif = *(unsigned char *)(s1+i) - *(unsigned char *)(s2+i);
    if (dif != 0)
      return dif;
  }
  return dif;
  /* panic("Not implemented"); */
}

#endif
