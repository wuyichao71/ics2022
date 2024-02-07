#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

/* wuyc */
#define STACK_NR_PAGE 8
#define STACK_SIZE (STACK_NR_PAGE * PGSIZE)
/* #define STACK_SIZE (8 * PGSIZE) */
/* wuyc */

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    Context *cp;
    AddrSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;

#endif
