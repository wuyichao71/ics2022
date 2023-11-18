#ifndef __LOADER_H__
#define __LOADER_H__
#include <common.h>
#if defined(__ISA_AM_NATIVE__)
# define EXCEPT_TYPE EM_X86_64
#elif defined(__ISA_X86__) || defined(__ISA_X86_64__)
# define EXCEPT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define EXCEPT_TYPE EM_RISCV
#elif defined(__ISA_MIPS32__)
# define EXCEPT_TYPE EM_MIPS
#else
# error Unsupported ISA
#endif

void naive_uload(PCB *pcb, const char *filename);

#endif
