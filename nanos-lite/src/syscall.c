#include <common.h>
#include "syscall.h"

/* wuyc */
#define STRACE(SYS_type, format, ...) printf(#SYS_type "(" format ")", ## __VA_ARGS__)
#define STRACE_OUT(ret) printf("     = %d\n", ret)
void sys_exit(int code);
/* wuyc */
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    /* wuyc */
    case SYS_exit: STRACE(SYS_exit, "%d", c->GPR2); sys_exit(c->GPR2); break;
    /* case SYS_exit: c->GPRx=0; break; */
    case SYS_yield: STRACE(SYS_yield, ""); yield(); c->GPRx = 0; STRACE_OUT(c->GPRx); break;
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
