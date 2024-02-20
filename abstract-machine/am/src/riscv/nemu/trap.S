#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)
#define KERNEL 0
#define USER 1

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((32 + 3 + 1) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  (32 * XLEN)
#define OFFSET_STATUS (33 * XLEN)
#define OFFSET_EPC    (34 * XLEN)
#define OFFSET_NP     (36 * XLEN)
#define OFFSET_T0     (5 * XLEN)

.align 3
.globl __am_asm_trap
__am_asm_trap:
  csrrw sp, mscratch, sp
  bnez sp, set_np
  csrr sp, mscratch # ksp == 0

  # wuyc
  addi sp, sp, -XLEN
  # wuyc
  addi sp, sp, -CONTEXT_SIZE
  STORE t0, OFFSET_T0(sp)
  STORE zero, OFFSET_NP(sp)
  j save_context

set_np:
  # wuyc
  addi sp, sp, -XLEN
  # wuyc
  addi sp, sp, -CONTEXT_SIZE

  STORE t0, OFFSET_T0(sp)
  li t0, USER
  STORE t0, OFFSET_NP(sp) 

save_context:

  csrr t0, mscratch
  STORE t0, OFFSET_SP(sp) # c->sp = $sp
  csrw mscratch, zero # support re-entry of CTE
  LOAD t0, OFFSET_T0(sp)

  MAP(REGS, PUSH)

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)

  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1

  mv a0, sp
  jal __am_irq_handle
  mv sp, a0

  # LOAD t0, OFFSET_SP(sp)
  # csrw mscratch, t0
  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  csrw mstatus, t1
  csrw mepc, t2

  MAP(REGS, POP)

  STORE t0, OFFSET_T0(sp)
  LOAD t0, OFFSET_NP(sp)
  
  beqz t0, set_sp
  addi sp, sp, CONTEXT_SIZE
  # wuyc
  addi sp, sp, XLEN
  # wuyc
  csrw mscratch, sp
  # wuyc
  addi sp, sp, -XLEN
  # wuyc
  addi sp, sp, -CONTEXT_SIZE

set_sp:
  LOAD t0, OFFSET_T0(sp)
  LOAD sp, OFFSET_SP(sp)
  mret
