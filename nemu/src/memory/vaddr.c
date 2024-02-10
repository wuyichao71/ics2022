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

#define TRANSLATE_ADDR(p_addr, vaddr, type) { \
  switch (isa_mmu_check(v_addr, len, MEM_TYPE_##type)) \
  { \
    case MMU_DIRECT: p_addr = v_addr; break; \
    case MMU_TRANSLATE: p_addr = isa_mmu_translate(v_addr, len, MEM_TYPE_##type); break; \
    default: panic("ERROR MMU MODE!") \
  } \
} while (0)

word_t vaddr_ifetch(vaddr_t addr, int len) {
  paddr_t paddr;
  TRANSLATE_ADDR(paddr, addr, IFETCH);
  return paddr_read(paddr, len);
  /* return paddr_read(addr, len); */
}

word_t vaddr_read(vaddr_t addr, int len) {
  /* wuyc */
  paddr_t paddr;
  TRANSLATE_ADDR(paddr, addr, READ);
  return paddr_read(paddr, len);
  /* return paddr_read(addr, len); */
  /* word_t ret = paddr_read(addr, len); */
  /* return ret; */
  /* wuyc */
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  /* wuyc */
  paddr_t paddr;
  TRANSLATE_ADDR(paddr, addr, WRITE);
/* #ifdef CONFIG_MTRACE */
/*   if(in_mtrace(addr)) */
/*     Log_cyan("write " FMT_WORD " to " FMT_PADDR, data, addr); */
/* #endif */
  /* wuyc */
  /* wuyc */
  paddr_write(paddr, len, data);
  /* paddr_write(addr, len, data); */
}
