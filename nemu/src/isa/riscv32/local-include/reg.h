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

#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}

#define gpr(idx) cpu.gpr[check_reg_idx(idx)]

static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

/* wuyc */
#define CSR_REG(f) f(MSTATUS, 0x300, = 0) f(MTVEC, 0x305) f(MEPC, 0x341) f(MCAUSE, 0x342) f(SATP, 0x180)
#define CSR_INDEX(name, code, ...) name __VA_ARGS__,
#define CSR_CODE(name, code, ...) name##_CODE = code,
#define CSR_CASE(name, code, ...) case name##_CODE: return name;
enum {
  CSR_REG(CSR_INDEX)
  NCSR
};
enum {
  CSR_REG(CSR_CODE)
};
static inline word_t code_to_csr(word_t code)
{
  switch (code)
  {
    CSR_REG(CSR_CASE)
    default: panic("Some CSR is missing!");
  }
}
/* wuyc */
#endif
