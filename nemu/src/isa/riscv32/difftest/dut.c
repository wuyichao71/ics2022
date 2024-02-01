/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"
/* wuyc */
#include <memory/paddr.h>
/* wuyc */

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  /* wuyc */
  for (int i = 0; i < ARRLEN(cpu.gpr); i++)
  {
    /* printf("hello\n"); */
    if (cpu.gpr[i] != ref_r->gpr[i])
    {
      printf("i = %d\n", i);
      printf("0x%08x\n", cpu.gpr[i]);
      printf("0x%08x\n", ref_r->gpr[i]);
      return false;
    }
  }
  if (ref_r->pc != cpu.pc)
  {
    printf("cpu.pc = 0x%08x\n", cpu.pc);
    printf("ref_r->pc = 0x%08x\n", ref_r->pc);
    return false;
  }
  return true;
  /* return false; */
  /* wuyc */
}

void isa_difftest_detach() {difftest_detach();}

static word_t code_to_csr(word_t code)
{
  switch (code)
  {
    case MSTATUS: return cpu.mstatus;
    case MTVEC: return cpu.mtvec;
    case MEPC: return cpu.mepc;
    case MCAUSE: return cpu.mcause;
    default: return 0;
  }
}
void isa_difftest_attach() {
  difftest_attach();
  CPU_state ref_r;
  /* bool success; */
  word_t inst;
  word_t csr_code[2] = {MCAUSE, MTVEC};//, MSTATUS, MEPC, MCAUSE};
  /* printf("here\n"); */
  for (int i = 0; i < ARRLEN(csr_code); i++)
  {
    /* ref_r = cpu; */
    ref_r.pc = RESET_VECTOR;
    printf("here\n");
    printf("0x%08x\n", code_to_csr(csr_code[i]));
    /* ref_r.gpr[isa_reg_str2val("a5", &success)] = code_to_csr(csr_code[i]); */
    ref_r.gpr[15] = code_to_csr(csr_code[i]);
    inst = 0x00079073 | csr_code[i] << 20;
    printf("0x%08x\n", inst);
    ref_difftest_memcpy(RESET_VECTOR, &inst, sizeof(word_t), DIFFTEST_TO_REF);
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_REF);
    ref_difftest_exec(1);
  }

  ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), CONFIG_MBASE+CONFIG_MSIZE-RESET_VECTOR, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}
