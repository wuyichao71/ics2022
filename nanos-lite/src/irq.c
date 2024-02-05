#include <common.h>
/* wuyc */
void do_syscall(Context *c);
Context* schedule(Context *prev);
/* wuyc */

static Context* do_event(Event e, Context* c) {
  /* printf("%d\n", c->GPR1); */
  switch (e.event) {
    /* wuyc */
    case EVENT_SYSCALL: do_syscall(c); break;
    case EVENT_YIELD: c = schedule(c); break;
    /* wuyc */

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
