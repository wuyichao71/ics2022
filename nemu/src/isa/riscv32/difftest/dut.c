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
  /* CPU_state ref_r = cpu; */
  /* CPU_state ref_r = {}; */
  word_t csr_code = {MSTATUS, MTVEC, MEPC, MCAUSE};
  /* bool success; */
  /* uint32_t inst[10] = {0x800017b7, 0x47878793, 0x30579073, 0x00027b7, */ 
    /* 0x80078793, 0x30079073, 0x342022f3, 0x30002373, 0x341023f3, 0x30502473}; */
  /* uint32_t init_inst[2] = {0x30579073, 0x30079073}; */
  /* uint32_t inst[5] = {0x00000000, 0x342022f3, 0x30002373, 0x341023f3, 0x30502473}; */
  /* uint32_t a5[2] = {0x80001478, 0x00001800}; */
  for (int i = 0; i < ARRLEN(csr_code); i++)
  {
    word_t lui, addi;
    word_t csr_data = code_to_csr(csr_code[i]);
    lui = (csr_data + 0x800) & (~0xfff);
    addi = (csr_data - lui);
    printf("$test = %d\n", csr_data);
    printf("0x%08x 0x%08x\n", lui, addi)
  }
  /* ref_r.pc = RESET_VECTOR; */
  /* ref_difftest_memcpy(RESET_VECTOR, inst, sizeof(uint32_t) * 10, DIFFTEST_TO_REF); */
  /* ref_difftest_regcpy(&ref_r, DIFFTEST_TO_REF); */
  /* ref_difftest_exec(10); */
  /* for (int i = 0; i < 2; i++) */
  /* { */
  /*   ref_r.pc = RESET_VECTOR; */
  /*   ref_r.gpr[15] = a5[i]; */
  /*   inst[0] = init_inst[i]; */
  /*   ref_difftest_memcpy(RESET_VECTOR, inst, sizeof(uint32_t) * 5, DIFFTEST_TO_REF); */
  /*   ref_difftest_regcpy(&ref_r, DIFFTEST_TO_REF); */
  /*   ref_difftest_exec(5); */
  /*   ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT); */
  /*   printf("ref_mcause = 0x%08x\n", ref_r.gpr[5]); */
  /*   printf("ref_mstatus = 0x%08x\n", ref_r.gpr[6]); */
  /*   printf("ref_mepc = 0x%08x\n", ref_r.gpr[7]); */
  /*   printf("ref_mtvec = 0x%08x\n", ref_r.gpr[8]); */
  /* } */
  /* word_t csr_code[2] = {MTVEC, MSTATUS};//, MSTATUS, MEPC, MCAUSE}; */
  /* /1* printf("here\n"); *1/ */
  /* for (int i = 0; i < ARRLEN(csr_code); i++) */
  /* { */
  /*   ref_r = cpu; */
  /*   ref_r.pc = RESET_VECTOR; */
  /*   printf("here\n"); */
  /*   printf("0x%08x\n", code_to_csr(csr_code[i])); */
  /*   /1* ref_r.gpr[isa_reg_str2val("a5", &success)] = code_to_csr(csr_code[i]); *1/ */
  /*   ref_r.gpr[5] = code_to_csr(csr_code[i]); */
  /*   inst[0] = 0x00029073 | (csr_code[i] << 20); */
  /*   inst[1] = 0x342022f3; */
  /*   inst[2] = 0x30002373; */
  /*   inst[3] = 0x341023f3; */
  /*   inst[4] = 0x30502473; */
  /*   printf("0x%08x\n", inst[0]); */
  /*   ref_difftest_memcpy(RESET_VECTOR, &inst, sizeof(word_t) * 5, DIFFTEST_TO_REF); */
  /*   ref_difftest_regcpy(&ref_r, DIFFTEST_TO_REF); */
  /*   ref_difftest_exec(5); */
  /*   ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT); */
  /*   printf("ref_mcause = 0x%08x\n", ref_r.gpr[5]); */
  /*   printf("ref_mstatus = 0x%08x\n", ref_r.gpr[6]); */
  /*   printf("ref_mepc = 0x%08x\n", ref_r.gpr[7]); */
  /*   printf("ref_mtvec = 0x%08x\n", ref_r.gpr[8]); */
  /* } */ 
  /* ref_r.pc = RESET_VECTOR + 8; */
  /* printf("here\n"); */
  /* printf("0x%08x\n", code_to_csr(csr_code[i])); */
  /* ref_r.gpr[isa_reg_str2val("a5", &success)] = code_to_csr(csr_code[i]); */
  /* ref_r.gpr[15] = code_to_csr(csr_code[i]); */
  /* inst = 0x00000073; */
  /* printf("0x%08x\n", inst); */
  /* ref_difftest_memcpy(RESET_VECTOR + 8, &inst, 4, DIFFTEST_TO_REF); */
  /* ref_difftest_regcpy(&ref_r, DIFFTEST_TO_REF); */
  /* ref_difftest_exec(1); */

  difftest_attach();
  ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), CONFIG_MBASE+CONFIG_MSIZE-RESET_VECTOR, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}
