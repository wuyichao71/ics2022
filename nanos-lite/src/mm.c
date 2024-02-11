#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *prev_pf = pf;
  pf = pf + nr_page * PGSIZE;
  return prev_pf;
  /* return NULL; */
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  void *ptr = new_page(n / PGSIZE);
  memset(ptr, 0, n);
  return ptr;
  /* return NULL; */
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
