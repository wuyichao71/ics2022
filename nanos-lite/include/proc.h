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
/* wuyc */
#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80
/* wuyc */

#endif
