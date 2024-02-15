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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define CSR_REG_PRINT(name, code, ...) printf("%4s: " FMT_WORD "\n", #name, cpu.csr[name]);
void difftest_reg_display(CPU_state ref_r)
{
  int regs_len = ARRLEN(regs);
  printf("IN DIFFTEST\n");
  for(int i = 0; i < regs_len; i++)
  {
    printf("%4s: " FMT_WORD, reg_name(i, 4), ref_r.gpr[check_reg_idx(i)]);
    if ((i + 1) % 4 == 0)
      printf("\n");
  }
  printf("%4s: " FMT_WORD "\n", "PC", ref_r.pc);
  /* CSR_REG(CSR_REG_PRINT); */
}
void isa_reg_display() {
  /* wuyc */
  /* int regs_len = sizeof(regs) / sizeof(char *); */
  int regs_len = ARRLEN(regs);
  for(int i = 0; i < regs_len; i++)
  {
    printf("%4s: " FMT_WORD, reg_name(i, 4), gpr(i));
    if ((i + 1) % 4 == 0)
      printf("\n");
  }
  printf("%4s: " FMT_WORD "\n", "PC", cpu.pc);
  /* printf("%4s: " FMT_WORD "\n", "MCAUSE", cpu.csr[MCAUSE]); */
  /* printf("%4s: " FMT_WORD "\n", "MSTATUS", cpu.csr[MSTATUS]); */
  /* printf("%4s: " FMT_WORD "\n", "MEPC", cpu.csr[MEPC]); */
  /* printf("%4s: " FMT_WORD "\n", "STAP", cpu.csr[SATP]); */
  CSR_REG(CSR_REG_PRINT);

  /* wuyc */
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int regs_len = ARRLEN(regs);
  for(int i = 0; i < regs_len; i++)
  {
    if (strcmp(s, regs[i]) == 0)
    {
      *success = true;
      return gpr(i);
    }
  }
  if (strcmp(s, "pc") == 0)
  {
      *success = true;
      return cpu.pc;
  }

  *success = false;
  return 0;
}
