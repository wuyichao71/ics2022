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
    if (cpu.gpr[i] != ref_r->gpr[i])
    {
      printf("0x%08x\n", cpu.gpr[i]);
      printf("0x%08x\n", ref_r->gpr[i]);
      return false;
    }
  }
  if (ref_r->pc != cpu.pc)
    return false;
  return true;
  /* return false; */
  /* wuyc */
}

void isa_difftest_detach() {difftest_detach();}

void isa_difftest_attach() {
  difftest_attach();
  ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), CONFIG_MSIZE, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}
