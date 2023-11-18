#include <proc.h>
#include <elf.h>
/* wuyc */
#include <ramdisk.h>
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
  Elf_Ehdr ehdr;
  Elf_Phdr *phdr;
  int phsize;

  ramdisk_read(&ehdr, 0, sizeof(ehdr));

  phsize = ehdr.e_phnum * sizeof(Elf_Phdr);
  phdr = (Elf_Phdr *)malloc(phsize);

  ramdisk_read(phdr, ehdr.e_phoff, phsize);
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    printf("%d\n", phdr[i].p_filesz);
  }
  /* printf("Size of ph: %d\n", ehdr.e_phnum * sizeof(Elf_Phdr)); */
  free(phdr);
  /* printf("Elf_Ehdr = %p\n", ehdr.e_entry); */

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

