#include <common.h>
/* wuyc */
#include "syscall.h"
/* wuyc */

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    /* wuyc */
    case EVENT_SYSCALL: do_syscall(c); break;
    case EVENT_YIELD: printf("event is %d\n", e.event); break;
    /* wuyc */

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
