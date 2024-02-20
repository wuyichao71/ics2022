#include <proc.h>
#include <elf.h>
/* wuyc */
#include <am.h>
#include <ramdisk.h>
#include <loader.h>
#include <fs.h>

/* wuyc */

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  /* TODO(); */
  int fd = fs_open(filename, 0, 0);
  /* printf("%d\n", fd); */
  Elf_Ehdr ehdr;
  /* int phsize; */
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  /* assert(*(uint32_t *)ehdr.e_ident == 0xBadC0de); */
  /* printf("0x%x\n", *(uint32_t *)ehdr.e_ident); */
  assert(*(uint32_t *)ehdr.e_ident == *(uint32_t *)"\x7F""ELF");
  assert(ehdr.e_machine == EXCEPT_TYPE);
  /* printf("0x%08x\n", ehdr.e_entry); */
  /* panic("Stop"); */
  Elf_Phdr phdr[ehdr.e_phnum];
  /* printf("%d\n", ehdr.e_phnum); */
  /* panic("Stop"); */
  /* Elf_Phdr *phdr = (Elf_Phdr *)(&ramdisk_start + ehdr.e_phoff); */
  size_t phsize = ehdr.e_phnum * sizeof(Elf_Phdr);
  /* printf("here\n"); */
  /* phdr = (Elf_Phdr *)malloc(phsize); */
  /* printf("%d, %d\n", sizeof(Elf_Ehdr), ehdr.e_phoff); */
  /* panic("Stop"); */
  /* fs_read(fd, &ehdr, sizeof(Elf_Ehdr)); */
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  fs_read(fd, &phdr, phsize);
  /* panic("Stop"); */
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    /* printf("0x%08x\n", phdr[i].p_offset); */
    if (phdr[i].p_type == PT_LOAD)
    {
      fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
#ifdef HAS_VME
      /* int va = phdr[i].p_vaddr; */
      /* int ini = va & (PGSIZE - 1); */
      for(int read_len = 0; read_len < phdr[i].p_memsz;)
      {
        intptr_t va = phdr[i].p_vaddr + read_len;
        /* printf("va = 0x%08x\n", va); */
        int offset = va & (PGSIZE - 1);
        void *pa = new_page(1);
        memset(pa, 0, PGSIZE);
        /* printf("pa = 0x%08x\n", pa); */
        int len = PGSIZE - offset;
        map(&pcb->as, (void *)(va & ~(PGSIZE - 1)), pa, PTE_U | PTE_A | PTE_D | PTE_R | PTE_W | PTE_X | PTE_V);
        if (read_len < phdr[i].p_filesz)
        {
          int rlen = len;
          if (read_len + rlen > phdr[i].p_filesz) rlen = phdr[i].p_filesz - read_len;
          fs_read(fd, pa + offset, rlen);
          /* printf("rlen = 0x%08x\n", rlen); */
        }
        /* va += len; */
        read_len += len;
        /* printf("read_len = 0x%08x\n", read_len); */
      }
#else
      fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_filesz);
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
#endif
      /* printf("0x%08x\n", phdr[i].p_filesz); */
    }
  }
  /* printf("Size of ph: %d\n", ehdr.e_phnum * sizeof(Elf_Phdr)); */
  /* free(phdr); */
  /* printf("Elf_Ehdr = %p\n", ehdr.e_entry); */

  /* panic("Stop"); */
  fs_close(fd);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  /* Area kstack = {0}; */
  Area kstack = RANGE(pcb->stack, pcb->stack + STACK_SIZE);
  /* kstack.start = pcb->stack; */
  /* kstack.end = kstack.start + STACK_SIZE; */
  pcb->cp = kcontext(kstack, entry, arg);
}

inline static int arg_number(char *const argv[])
{
  int i = 0;
  for (; argv[i] != NULL; i++);
  return i;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  Area kstack = RANGE(pcb->stack, pcb->stack + STACK_SIZE);
  char *ustack = new_page(STACK_NR_PAGE) + STACK_SIZE;
  /* printf("here\n"); */
#ifdef HAS_VME
  protect(&pcb->as);
  /* char *ustack = pcb->as.area.end; */
  char *ustack_top = ustack;
  void *pa = ustack;
  void *va = pcb->as.area.end;
  for (int i = 0; i < STACK_NR_PAGE; i++)
  {
    va -= PGSIZE;
    pa -= PGSIZE;
    /* printf("va = 0x%08x\n", va); */
    /* printf("pa = 0x%08x\n", pa); */
    map(&pcb->as, va, pa, PTE_U | PTE_D | PTE_A | PTE_R | PTE_W | PTE_X | PTE_V);
  }
#endif
  int argc = argv == NULL ? 0 : arg_number(argv);
  int envc = envp == NULL ? 0 : arg_number(envp);
  /* char **argv_p = NULL; */
  /* char **envp_p = NULL; */
  /* if (argc != 0) */
  /*   argv_p = (char **)malloc(argc * sizeof(char *)); */
  /* if (envc != 0) */
  /*   envp_p = (char **)malloc(envc * sizeof(char *)); */
  char *envp_p[envc];
  char *argv_p[argc];
  for (int i = envc - 1; i >= 0; i--)
  {
    int length = strlen(envp[i]) + 1;
    ustack -= length;
    strcpy(ustack, envp[i]);
    envp_p[i] = ustack;
    /* printf("%s\n", envp[i]); */
  }
  for (int i = argc - 1; i >= 0; i--)
  {
    int length = strlen(argv[i]) + 1;
    ustack -= length;
    strcpy(ustack, argv[i]);
    argv_p[i] = ustack;
  }

  char **ustack2 = (char **)((uintptr_t)ustack & ~(sizeof(char **) - 1));
  ustack2--;
  *(ustack2--) = NULL;
  for (int i = envc - 1; i >= 0; i--)
  {
    *(ustack2--) = envp_p[i];
  }
  /* free(envp_p); */

  *ustack2 = NULL;
  for (int i = argc - 1; i >= 0; i--)
  {
    ustack2--;
    *ustack2 = argv_p[i];
  }
  /* free(argv_p); */

  int *ustack3 = (int *)ustack2;
  ustack3--;
  *ustack3 = argc;

  /* printf("%d\n", argc); */
  /* printf("%d\n", envc); */
  /* int p_argc = *ustack3; */
  /* printf("argc = %d\n", p_argc); */
  /* ustack3++; */
  /* char **ustack4 = (char **)ustack3; */
  /* for (; *ustack4 != NULL; ustack4++) */
  /*   printf("%s\n", *ustack4); */
  /* ustack4++; */
  /* printf("----------------------\n"); */
  /* for (; *ustack4 != NULL; ustack4++) */
  /*   printf("%s\n", *ustack4); */

  /* panic("JUST end!"); */
  /* kstack.start = pcb->stack; */
  /* kstack.end = kstack.start + STACK_SIZE; */
  uintptr_t entry = loader(pcb, filename);
  /* AddrSpace as = {}; */
  pcb->cp = ucontext(&pcb->as, kstack, (void *)entry);
#ifdef HAS_VME
  /* printf("ustack3 = 0x%08x\n", ustack3); */
  /* printf("ustack_top = 0x%08x\n", ustack_top); */
  /* printf("pcb->as.area.end = 0x%08x\n", pcb->as.area.end); */
  pcb->cp->GPRx = (intptr_t)pcb->as.area.end - ((intptr_t)ustack_top - (intptr_t)ustack3);
#else
  pcb->cp->GPRx = (uintptr_t)ustack3;
#endif
  pcb->max_brk = 0;
}
