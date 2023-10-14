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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

/* wuyc */
#ifdef CONFIG_FTRACE
static word_t level = 0;
#endif
/* wuyc */

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  /* TYPE_I, TYPE_U, TYPE_S, */
  /* wuyc */
  TYPE_I, TYPE_U, TYPE_S, TYPE_J, TYPE_R, TYPE_B,
  /* wuyc */
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
/* wuyc */
/* #define immJ() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 19) | \ */
/*     (SEXT(BITS(i, 19, 12), 8) << 11) | \ */
/*     (SEXT(BITS(i, 20, 20), 1) << 10) | \ */
/*     (BITS(i, 30, 21))) << 1; } while(0) */
/* #define immB() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 11) | \ */
/*     (SEXT(BITS(i, 7, 7), 1) << 10) | \ */
/*     (SEXT(BITS(i, 30, 25), 6) << 4) | \ */
/*     (BITS(i, 11, 8))) << 1; } while(0) */
#define immJ() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 19) | \
    (BITS(i, 19, 12) << 11) | \
    (BITS(i, 20, 20) << 10) | \
    (BITS(i, 30, 21))) << 1; } while(0)
#define immB() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 11) | \
    (BITS(i, 7, 7) << 10) | \
    (BITS(i, 30, 25) << 4) | \
    (BITS(i, 11, 8))) << 1; } while(0)
/* wuyc */

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    /* wuyc */
    case TYPE_J: src1R();          immJ(); break;
    case TYPE_R: src1R(); src2R()        ; break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    /* wuyc */
  }
}

/* wuyc */
#ifdef CONFIG_FTRACE
#define IS_RA(i) (i == 1 || i == 5)

static inline void write_header(vaddr_t pc)
{
  log_write(FMT_WORD ": ", pc);
  for(int i = 0; i < level; i++)
  {
    log_write("  ");
  }
}

static inline int find_func(vaddr_t pc)
{
  int name_ndx = 0;
  for(int i = 0; i < func_num; i++)
  {
    int pc_diff = pc - func_hdr[i].st_value;
    if (pc_diff >= 0 && pc_diff < func_hdr[i].st_size)
    {
      name_ndx = func_hdr[i].st_name;
    }
  }
  return name_ndx;
}

static void write_function(Decode *s)
{
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rd  = BITS(i, 11, 7);
  /* printf("rs1 = %d, rd = %d\n", rs1, rd); */

  if (!IS_RA(rd) && IS_RA(rs1))
  {
    level--;
    write_header(s->pc);
    log_write("ret  [%s]\n", strtab + find_func(s->pc));
  }
  else if (IS_RA(rd) && !IS_RA(rs1))
  {
    write_header(s->pc);
    log_write("call [%s@" FMT_WORD "]\n", strtab + find_func(s->dnpc), s->dnpc);
    level++;
  }
  else if (IS_RA(rd) && IS_RA(rs1) && rd != rs1)
  {
    level--;
    write_header(s->pc);
    log_write("ret  []\n");
    write_header(s->pc);
    log_write("call [@" FMT_WORD "]\n", s->dnpc);
    level++;
  }
  else if (IS_RA(rd) && IS_RA(rs1) && rd == rs1)
  {
    write_header(s->pc);
    log_write("call [@" FMT_WORD "]\n", s->dnpc);
    level++;
  }
}

#define JUMP(...) do { \
  __VA_ARGS__; \
  write_function(s); \
  } while(0)
  /* printf("0x%8x, 0x%8x\n", s->pc, s->dnpc); \ */

#else

#define JUMP(...) do { __VA_ARGS__; } while(0)

#endif

/* wuyc */

static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  /* wuyc */
  /* U type */
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  /* wuyc */
  /* wuyc */
  /* J type */
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, JUMP(R(rd) = s->pc + 4; s->dnpc = s->pc + imm));
  /* wuyc */
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr(src1 + imm, 4));
  /* wuyc */
  /* I type */
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, JUMP(s->dnpc = (src1 + imm) & ~1; R(rd) = s->pc + 4));
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = (src1 < imm));
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (sword_t)src1 >> imm);
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = src1 >> imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", and    , I, R(rd) = src1 & imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << imm);
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2));
  /* wuyc */
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  /* wuyc */
  /* S type */
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  /* wuyc */
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd) = SEXT(Mr(src1 + imm, 1), 8));

  /* wuyc */
  /* R type */
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = src1 < src2);
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = (sword_t)src1 < (sword_t)src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 110 ????? 00100 11", ori    , R, R(rd) = src1 | imm);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = src1 * src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = (sword_t)src1 / (sword_t)src2);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = (sword_t)src1 % (sword_t)src2);
  /* INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = ((int64_t)src1 * (int64_t)src2) >> 32); */
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = (SEXT(src1, 32) * SEXT(src2, 32)) >> 32);
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(rd) = (EXT(src1, 32) * EXT(src2, 32)) >> 32);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = src1 % src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = src1 / src2);
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (sword_t)src1 >> src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = src1 >> src2);
  /* wuyc */
  /* wuyc */
  /* B type */
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if (src1 == src2) s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if (src1 != src2) s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if ((sword_t)src1 >= (sword_t)src2) s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if ((sword_t)src1 < (sword_t)src2) s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, if (src1 >= src2) s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", blt    , B, if (src1 < src2) s->dnpc = s->pc + imm);
  /* INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if (src1 >= src2) s->dnpc = s->pc + imm); */
  /* INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if (src1 < src2) s->dnpc = s->pc + imm); */
  /* wuyc */

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
