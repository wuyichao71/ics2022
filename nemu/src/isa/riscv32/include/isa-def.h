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

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>
/* wuyc */
#include "../local-include/reg.h"
/* wuyc */

typedef struct {
  word_t gpr[32];
  vaddr_t pc;
  /* wuyc */
  word_t csr[NCSR];
  int priv_mode;
  bool INTR;
  /* word_t mtvec; */
  /* vaddr_t mepc; */
  /* word_t mstatus; */
  /* word_t mcause; */
  /* wuyc */
} riscv32_CPU_state;

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv32_ISADecodeInfo;

/* wuyc */
#define PGSHIFT 12
enum {MODE_U = 0x00, MODE_S = 0x01, MODE_M = 0x11};
/* wuyc */
/* #define isa_mmu_check(vaddr, len, type) (MMU_DIRECT) */
#define isa_mmu_check(vaddr, len, type) ((vaddr >> PGSHIFT == (vaddr + len - 1) >> PGSHIFT) ? \
    cpu.csr[SATP] >> 31 : MMU_FAIL)

#endif
