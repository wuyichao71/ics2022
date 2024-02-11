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

extern char _data;
extern char _bss_start;
extern char _stack_top;
extern char _end;
void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
  /* wuyc */
  printf("_data = 0x%08x\n", &_data);
  printf("_bss_start = 0x%08x\n", &_bss_start);
  printf("_stack_top = 0x%08x\n", &_stack_top);
  printf("_end = 0x%08x\n", &_end);
  /* wuyc */
}
