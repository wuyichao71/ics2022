#include <common.h>
#include "syscall.h"

/* wuyc */
#define __STRACE__
#ifdef __STRACE__
# define SYS_format(format) format "         = %d"
# define STRACE(SYS_type, format, ...) printf(#SYS_type format "\n", ## __VA_ARGS__)
#else
# define SYS_format(format) 
# define STRACE(SYS_type, format, ...) 
#endif

void sys_exit(int code);
size_t sys_write(int fd, const void *buf, size_t count);
uintptr_t sys_yield();
/* wuyc */
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  /* wuyc */
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  /* wuyc */

  switch (a[0]) {
    /* wuyc */
    case SYS_exit: STRACE(sys_exit, "(%d)", a[1]); sys_exit(a[1]); break;
    /* case SYS_exit: c->GPRx=0; break; */
    case SYS_yield: c->GPRx = sys_yield(); STRACE(sys_yield, SYS_format("()"), c->GPRx); break;
    case SYS_write: c->GPRx = sys_write(a[1], (const void *)a[2], a[3]); 
                    STRACE(sys_write, SYS_format("(%d, 0x%08x, %d)"), a[1], a[2], a[3], c->GPRx); break;
    /* wuyc */
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  /* printf("HHH\n"); */
}

uintptr_t sys_yield()
{
  yield();
  return (uintptr_t)0;
}

/* wuyc */
void sys_exit(int code)
{
  halt(code);
}

size_t sys_write(int fd, const void *buf, size_t count)
{
  if (fd == 1 || fd == 2)
  {
    for (int i = 0; i < count; i++)
    {
      putch(((char *)buf)[i]);
    }
  }
  return count;
}
/* wuyc */
