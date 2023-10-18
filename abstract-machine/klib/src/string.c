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
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
  /* panic("Not implemented"); */
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for(i = 0; (dst[dst_len+i] = src[i]) != '\0'; i++);
  return dst;
  /* panic("Not implemented"); */
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  for(; s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0'; i++);
  return s1[i] - s2[i];
  /* panic("Not implemented"); */
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
  for(; s1[i] == s2[i] && i < n && s1[i] != '\0' && s2[i] != '\0'; i++);
  return s1[i] - s2[i];
  /* panic("Not implemented"); */
}

void *memset(void *s, int c, size_t n) {
  for(size_t i = 0; i < n; i++) *(uint8_t *)(s+i) = c;
  return s;
  /* panic("Not implemented"); */
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  for(size_t i = 0; i < n; i++)
    *(uint8_t *)(out + i) = *(uint8_t *)(in + i);
  return out;
  /* panic("Not implemented"); */
}

int memcmp(const void *s1, const void *s2, size_t n) {
  /* int dif = 0; */
  size_t i = 0;
  for(; i < n && *(uint8_t *)(s1+i) == *(uint8_t *)(s2+i); i++);
  if (i == n)
    return 0;
  else
    return *(char *)(s1+i) - *(char *)(s2+i);
  /* panic("Not implemented"); */
}

#endif
