#include <proc.h>
/* wuyc */
#include <loader.h>
/* wuyc */

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, NULL);
  switch_boot_pcb();

  Log("Initializing processes...");
  /* wuyc */
  /* naive_uload(NULL, "/bin/file-test"); */
  /* naive_uload(NULL, "/bin/timer-test"); */
  /* naive_uload(NULL, "/bin/event-test"); */
  /* naive_uload(NULL, "/bin/bmp-test"); */
  /* naive_uload(NULL, "/bin/nslider"); */
  /* naive_uload(NULL, "/bin/menu"); */
  /* naive_uload(NULL, "/bin/nterm"); */
  /* naive_uload(NULL, "/bin/typing-game"); */
  /* naive_uload(NULL, "/bin/dhrystone"); */
  /* naive_uload(NULL, "/bin/nplayer"); */
  /* naive_uload(NULL, "/bin/cpp-test"); */
  /* naive_uload(NULL, "/bin/pal"); */
  /* naive_uload(NULL, "/bin/coremark"); */
  /* naive_uload(NULL, "/bin/hello"); */
  /* wuyc */

  // load program here

}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = &pcb[0];
  return current->cp;
  /* return NULL; */
}
