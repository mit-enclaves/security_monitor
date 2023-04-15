#include <sm_constants.h>

.option norvc
.section .text.trap_helpers

# TODO: turn this into a nice macro instead of a call maybe
.globl .save_regs_and_set_args
.save_regs_and_set_args:
  # Preserve the registers.
  sd x0, (sp) # Zero x0's save slot.
  #sd ra, 1*REGBYTES(sp) # previously saved
  #sd sp, 2*REGBYTES(sp) # separately handled
  sd gp, 3*REGBYTES(sp)
  sd tp, 4*REGBYTES(sp)
  # sd t0, 5*REGBYTES(sp) # previously saved
  sd t1, 6*REGBYTES(sp)
  sd t2, 7*REGBYTES(sp)
  sd s0, 8*REGBYTES(sp)
  sd s1, 9*REGBYTES(sp)
  sd a0,10*REGBYTES(sp)
  sd a1,11*REGBYTES(sp)
  sd a2,12*REGBYTES(sp)
  sd a3,13*REGBYTES(sp)
  sd a4,14*REGBYTES(sp)
  sd a5,15*REGBYTES(sp)
  sd a6,16*REGBYTES(sp)
  sd a7,17*REGBYTES(sp)
  sd s2,18*REGBYTES(sp)
  sd s3,19*REGBYTES(sp)
  sd s4,20*REGBYTES(sp)
  sd s5,21*REGBYTES(sp)
  sd s6,22*REGBYTES(sp)
  sd s7,23*REGBYTES(sp)
  sd s8,24*REGBYTES(sp)
  sd s9,25*REGBYTES(sp)
  sd s10,26*REGBYTES(sp)
  sd s11,27*REGBYTES(sp)
  sd t3,28*REGBYTES(sp)
  sd t4,29*REGBYTES(sp)
  sd t5,30*REGBYTES(sp)
  sd t6,31*REGBYTES(sp)
  # Handler arguments: (uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
  mv a0, sp        # a0 <- uintptr_t* regs
  csrr a1, mcause  # a1 <- uintptr_t  mcause
  csrr a2, mepc    # a0 <- uintptr_t  mepc
  ret

# TODO: turn this into a nice macro instead of a call maybe
.globl .restore_regs
.restore_regs:
  # Restore all of the registers.
  # x0 need not be restored
  #ld ra, 1*REGBYTES(sp) # loaded later
  #ld sp, 2*REGBYTES(sp) # separately hadled
  ld gp, 3*REGBYTES(sp)
  ld tp, 4*REGBYTES(sp)
  # ld t0, 5*REGBYTES(sp) # loaded later
  ld t1, 6*REGBYTES(sp)
  ld t2, 7*REGBYTES(sp)
  ld s0, 8*REGBYTES(sp)
  ld s1, 9*REGBYTES(sp)
  ld a0,10*REGBYTES(sp)
  ld a1,11*REGBYTES(sp)
  ld a2,12*REGBYTES(sp)
  ld a3,13*REGBYTES(sp)
  ld a4,14*REGBYTES(sp)
  ld a5,15*REGBYTES(sp)
  ld a6,16*REGBYTES(sp)
  ld a7,17*REGBYTES(sp)
  ld s2,18*REGBYTES(sp)
  ld s3,19*REGBYTES(sp)
  ld s4,20*REGBYTES(sp)
  ld s5,21*REGBYTES(sp)
  ld s6,22*REGBYTES(sp)
  ld s7,23*REGBYTES(sp)
  ld s8,24*REGBYTES(sp)
  ld s9,25*REGBYTES(sp)
  ld s10,26*REGBYTES(sp)
  ld s11,27*REGBYTES(sp)
  ld t3,28*REGBYTES(sp)
  ld t4,29*REGBYTES(sp)
  ld t5,30*REGBYTES(sp)
  ld t6,31*REGBYTES(sp)
  ret

.globl .perform_mret_ecall
.perform_mret_ecall:
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0

.globl .perform_mret
.perform_mret:
  # Go back whence we came.
  ld t0, 2*REGBYTES(sp)
  csrw mscratch, t0

  ld ra, 1*REGBYTES(sp)
  ld t0, 5*REGBYTES(sp)
  csrrw sp, mscratch, sp
  csrw CSR_MFLUSH, zero
  csrw CSR_MSPEC, zero
  mret
