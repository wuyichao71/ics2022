#include <common.h>
#include "syscall.h"

/* wuyc */
#define SYS_format(format) format "         = %d"
#define STRACE(SYS_type, format, ...) printf(#SYS_type format "\n", ## __VA_ARGS__)
void sys_exit(int code);
/* wuyc */
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  /* wuyc */
  a[1] = c->GPR2;
  /* wuyc */

  switch (a[0]) {
    /* wuyc */
    case SYS_exit: STRACE(sys_exit, "(%d)", c->GPR2); sys_exit(c->GPR2); break;
    /* case SYS_exit: c->GPRx=0; break; */
    case SYS_yield: yield(); c->GPRx = 0; STRACE(sys_yield, SYS_format("()"), c->GPRx); break;
    case SYS_write: break;
    /* wuyc */
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  /* printf("HHH\n"); */
}

/* wuyc */
void sys_exit(int code)
{
  halt(code);
}
/* wuyc */
