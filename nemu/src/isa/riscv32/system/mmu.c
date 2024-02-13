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
#include <memory/paddr.h>
#include <memory/vaddr.h>
/* wuyc */
#include <common.h>
/* wuyc */

#define LEVELS 2
#define SHIFT_VPN(vaddr, shift) (((uint32_t)(vaddr) >> (shift)) & 0x3ff)
#define VPN_WIDTH 10
#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  /* printf("satp = 0x%08x\n", cpu.csr[SATP]); */
  /* printf("ptr = 0x%08x\n", cpu.csr[SATP] << PGSHIFT); */
  paddr_t pte_base = cpu.csr[SATP] << PGSHIFT;
  int shift = 32 - VPN_WIDTH;
  int index;
  paddr_t paddr;
  word_t pte;
  for (int i = LEVELS - 1; i > 0; i--)
  {
    index = SHIFT_VPN(vaddr, shift);
    pte = paddr_read(pte_base + index * sizeof(word_t), sizeof(word_t));
    assert((pte & PTE_V) == PTE_V);
    assert((pte & (PTE_R | PTE_W | PTE_X)) == 0);
    pte_base = (pte << 2) & ~0xfff;
    shift -= VPN_WIDTH;
    /* printf("out pte = 0x%08x\n", pte); */
  }
  /* printf("shift = %d\n", shift); */
  index = SHIFT_VPN(vaddr, shift);
  pte = paddr_read(pte_base + index * sizeof(word_t), sizeof(word_t));
  paddr = (pte << 2) & ~0xfff;
  /* printf("vaddr = 0x%08x\n", vaddr); */
  if ((pte & PTE_V) == PTE_V) printf("vaddr = 0x%08x\npte = 0x%08x\n", vaddr, pte);
  Assert((pte & PTE_V) == PTE_V, "vaddr = 0x%08x\npte = 0x%08x\n", vaddr, pte);
  switch (type)
  {
    case MEM_TYPE_IFETCH: assert((pte & PTE_X) == PTE_X); break;
    case MEM_TYPE_READ: assert((pte & PTE_R) == PTE_R); break;
    case MEM_TYPE_WRITE: assert((pte & PTE_W) == PTE_W); break;
  }
  return paddr;
  /* return MEM_RET_FAIL; */
}
