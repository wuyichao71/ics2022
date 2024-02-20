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
/* wuyc */
#include "../local-include/reg.h"
/* wuyc */

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csr[MEPC] = epc;
  cpu.csr[MCAUSE] = NO;
  if (cpu.csr[MSTATUS] & MSTATUS_MIE)
    cpu.csr[MSTATUS] |= MSTATUS_MPIE;
  else
    cpu.csr[MSTATUS] &= (~MSTATUS_MPIE);
  cpu.csr[MSTATUS] &= (~MSTATUS_MIE);
  /* cpu.csr[MSTATUS] |= (cpu.csr[MSTATUS] & MSTATUS_MIE) */
#ifdef CONFIG_ETRACE
  log_write(FMT_WORD ": trigger exception(ID: %d)\n", cpu.csr[MEPC], cpu.csr[MCAUSE]);
  /* printf(FMT_WORD ": trigger exception(ID: %d)\n", cpu.mepc, cpu.mcause); */
#endif
  /* printf("0x%08x\n", cpu.mtvec); */
  /* printf("0x%08x\n", cpu.mcause); */
  /* cpu.pc = cpu.mtvec; */

  return cpu.csr[MTVEC];
}


word_t isa_query_intr() {
  return INTR_EMPTY;
  if (cpu.INTR && (cpu.csr[MSTATUS] & MSTATUS_MIE))
  {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
