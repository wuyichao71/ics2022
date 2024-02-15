#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  /* printf("kas.ptr = 0x%08x\n", kas.ptr); */

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, PTE_U | PTE_D | PTE_A | PTE_R | PTE_W | PTE_X | PTE_V);
    }
    /* wuyc */
    /* printf("-------------------\n"); */
    /* wuyc */
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  /* printf("c->pdir = 0x%08x\n", c->pdir); */
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
  /* intptr_t pdir = get_satp(); */
  /* printf("pdir = 0x%08x\n", pdir); */
  /* printf("--------------------------\n"); */
}

/* wuyc */
#define LEVELS 2
#define SHIFT_VPN(vaddr, shift) (((uint32_t)(vaddr) >> (shift)) & 0x3ff)
#define VPN_WIDTH 10
void map(AddrSpace *as, void *va, void *pa, int prot) {
  PTE *pte_base = as->ptr;
  int shift = __riscv_xlen - VPN_WIDTH;
  int index;
  /* PTE pte; */
  /* if ((uint32_t)va == 0x7ffff000) */
    /* printf("map_pa = 0x%08x\n", pa); */
  for (int i = LEVELS - 1; i > 0; i--)
  {
    index = SHIFT_VPN(va, shift);
    PTE pte = pte_base[index];
    if ((pte & PTE_V) != PTE_V)
    {
      uint32_t pg_ptr = (uint32_t)pgalloc_usr(PGSIZE) & ~0xfff;
      pte_base[index] = pg_ptr >> 2 | PTE_V;
      /* printf("pg_pte = 0x%08x\n", pte_base[index]); */
      pte_base = (PTE *)pg_ptr;
    }
    else
    {
      pte_base = (PTE *)((pte & ~0x3ff) << 2);
    }
    /* printf("%d\n", shift); */
    shift -= VPN_WIDTH;
  }
  index = SHIFT_VPN(va, shift);
  /* pte = pte_base[index]; */
  if ((prot & PTE_V) != PTE_V)
  {
    pte_base[index] &= ~PTE_V;
  }
  else
  {
    pte_base[index] = ((uint32_t)pa & ~0xfff) >> 2 | prot;
    /* printf("pte = 0x%08x\n", pte_base[index]); */
  }
  /* printf("shift = %d\n", shift); */
  /* printf("0x%08x\n", pte_base); */
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c = (Context *)kstack.end - 1;
  *c = (Context){0};
  c->mepc = (uintptr_t)entry;
  c->mstatus = 0xa0000;
  c->GPRSP = (uintptr_t)kstack.end;
  c->pdir = as->ptr;
  /* c->GPR2 = (uintptr_t)arg; */
  return c;
  /* return NULL; */
}
