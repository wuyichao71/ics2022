#include <proc.h>
#include <elf.h>
/* wuyc */
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
  printf("%d\n", fd);
  Elf_Ehdr ehdr;
  /* int phsize; */
  fs_read(fd, &ehdr, sizeof(ehdr));
  /* assert(*(uint32_t *)ehdr.e_ident == 0xBadC0de); */
  assert(*(uint32_t *)ehdr.e_ident == *(uint32_t *)"\x7F""ELF");
  assert(ehdr.e_machine == EXCEPT_TYPE);
  /* printf("0x%x\n", *(uint32_t *)ehdr.e_ident); */
  /* Elf_Phdr phdr[ehdr.e_phnum]; */
  Elf_Phdr *phdr = (Elf_Phdr *)(&ramdisk_start + ehdr.e_phoff);
  /* phsize = ehdr.e_phnum * sizeof(Elf_Phdr); */
  /* phdr = (Elf_Phdr *)malloc(phsize); */
  /* ramdisk_read(phdr, ehdr.e_phoff, phsize); */
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
      /* printf("0x%x\n", phdr[i].p_filesz); */
    }
  }
  /* printf("Size of ph: %d\n", ehdr.e_phnum * sizeof(Elf_Phdr)); */
  /* free(phdr); */
  /* printf("Elf_Ehdr = %p\n", ehdr.e_entry); */

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

