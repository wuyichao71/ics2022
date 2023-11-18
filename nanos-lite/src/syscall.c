#include <common.h>
#include "syscall.h"
void sys_exit(int code);
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    /* wuyc */
    /* case SYS_exit: sys_exit(c->GPR2);break; */
    case SYS_exit: break;
    case SYS_yield: yield(); c->GPRx = 0; break;
    /* wuyc */
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

/* wuyc */
void sys_exit(int code)
{
  halt(code);
}
/* wuyc */
