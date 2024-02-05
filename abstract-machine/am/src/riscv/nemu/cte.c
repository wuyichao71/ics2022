#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

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
  /* printf("0x%08x\n", c); */
  /* printf("0x%08x\n", &(c->gpr[1])); */
  /* wuyc */
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
      case 11:
        if (c->GPR1 == -1)
          ev.event = EVENT_YIELD;
        else
          ev.event = EVENT_SYSCALL;
        c->mepc += 4;
        break;
      /* wuyc */
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    /* printf("In %s: c->mepc = 0x%08x\n", __func__, c->mepc); */
    assert(c != NULL);
  }

  /* printf("DDD\n"); */
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  /* wuyc */
  asm volatile("csrw mstatus, %0" : : "r"(0x1800));
  /* wuyc */

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context *)kstack.end - 1;
  *c = (Context){0};
  c->mepc = (uintptr_t)entry;
  c->mstatus = 0x1800;
  c->GPRSP = (uintptr_t)kstack.end;
  c->GPR2 = (uintptr_t)arg;
  return c;
  /* return NULL; */
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
