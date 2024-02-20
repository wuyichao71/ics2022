#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

/* wuyc */
enum {
  IRQ_TIMER = 0x80000007,
  IRQ_M_ECALL = 11,
  IRQ_U_ECALL = 8,
  IRQ_S_ECALL = 9,
};
/* wuyc */


static Context* (*user_handler)(Event, Context*) = NULL;
/* wuyc */
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);
/* wuyc */

Context* __am_irq_handle(Context *c) {
  /* wuyc */
  /*
  for (int i = 0; i < 32; i++)
  {
    printf(" 0x%08x", c->gpr[i]);
    if ((i + 1) % 4 == 0)
      printf("\n");
  }
  printf("%4s: 0x%08x\n", "mcause", c->mcause);
  printf("%4s: 0x%08x\n", "mstatus", c->mstatus);
  printf("%4s: 0x%08x\n", "mepc", c->mepc);
  */
  printf("mcause = 0x%08x\n", c->mcause);
  printf("reason = %d\n", c->GPR1);
  /* wuyc */
  if (c->pdir != NULL)
    __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      /* wuyc */
      /* case 0: */ 
      /* case 1: */ 
      /* case 2: */ 
      /* case 3: */
      /* case 4: */
      /* case 7: */
      /* case 8: */
      /* case 9: */
      /* case 19: */
        /* ev.event = EVENT_SYSCALL; c->mepc += 4; break; */
      /* case -1: */ 
        /* ev.event = EVENT_YIELD  ; c->mepc += 4; break; */
      case IRQ_TIMER:
        ev.event = EVENT_IRQ_TIMER;
        break;
      case IRQ_U_ECALL:
      case IRQ_S_ECALL:
      case IRQ_M_ECALL:
        if (c->GPR1 == -1)
        {
          ev.event = EVENT_YIELD;
        }
        else
          ev.event = EVENT_SYSCALL;
        c->mepc += 4;
        break;
      /* wuyc */
      default: ev.event = EVENT_ERROR; break;
    }

    /* printf("type = %d\n", (int)c->GPR1); */
    c = user_handler(ev, c);
    /* printf("In %s: c->mepc = 0x%08x\n", __func__, c->mepc); */
    assert(c != NULL);
  }
  /* printf("DDD\n"); */
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  /* wuyc */
  /* asm volatile("csrw mstatus, %0" : : "r"(0x1800)); */
  asm volatile("csrw mstatus, %0" : : "r"(0x1800));
  /* asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap)); */
  /* asm volatile("csrw medeleg, %0" : : "r"(0x1 << 7)); */
  /* wuyc */

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context *)kstack.end - 1;
  *c = (Context){0};
  c->mepc = (uintptr_t)entry;
  c->mstatus = 0x1800 | MSTATUS_MPIE;
  c->GPRSP = (uintptr_t)kstack.end;
  c->GPR2 = (uintptr_t)arg;
  c->pdir = NULL;
  c->np = MODE_M;
  return c;
  /* return NULL; */
}

void yield() {
  asm volatile("li a7, -1; ecall");
  /* printf("Here"); */
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
