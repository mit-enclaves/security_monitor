#include <constants.h>

# TODO: Make sure stack never grows above 4KB !!

# Trap vector (table) for events originating from enclave
# ================================================================
  .section .text.enclave_trap
  .align 6 # Align to a cache line (64B) boundary
enclave_trap_table:
  # NOTE: only T0 are saved at this point
  .word .bad_event              //  0: misaligned instruction address
  .word .bad_event              //  1: instruction access fault
  .word .bad_event              //  2: illegal instruction
  .word .bad_event              //  3: breakpoint
  .word .bad_event              //  4: misaligned load addr
  .word .bad_event              //  5: load access fault
  .word .bad_event              //  6: misaligned store/AMO addr
  .word .bad_event              //  7: store/AMO access fault
  .word .ecall_from_e_trap      //  8: ECALL from U-mode
  .word .bad_event              //  9: ECALL from S-mode
  .word .bad_event              // 10: (reserved)
  .word .bad_event              // 11: ECALL from M-mode
  .word .bad_event              // 12: instruction page fault
  .word .bad_event              // 13: load page fault
  .word .bad_event              // 14: (reserved)
  .word .bad_event              // 15: store/AMO page fault

  .align 6
enclave_interrupt_table:
  # NOTE: only T0 are saved at this point
  .word .bad_event              //  0: user software interrupt
  .word .bad_event              //  1: supervisor software interrupt
  .word .bad_event              //  2: (reserved)
  .word .bad_event              //  3: machine softwre interrupt
  .word .bad_event              //  4: user timer interrupt
  .word .bad_event              //  5: supervisor timer interrupt
  .word .bad_event              //  6: (reserved)
  .word .bad_event              //  7: machine timer interrupt
  .word .bad_event              //  8: user external interrupt
  .word .bad_event              //  9: supervisor external interrupt
  .word .bad_event              // 10: (reserved)
  .word .bad_event              // 11: machine external interrupt

# Handler for events originating from an enclave
# ====================================================
  .section .text.enclave_trap.entry
  .globl trap_vector_from_enclave
trap_vector_from_enclave:

  # 1. Prepare to route the event: set up an M-mode stack, make a0, a1 usable.
  csrrw sp, mscratch, sp # sp <--> mscratch (save SP, restore M-mode SP)
  beqz sp, .recursive_trap_from_enclave
  # TODO: the above is problematic. Make sure recurison does not blow the stack.

  # Save ra, t0 and former SP, load the trap cause (mcause)
  sd ra, 1*REGBYTES(sp)
  sd t0, 5*REGBYTES(sp)
  csrr t0, mscratch
  sd t0, 2*REGBYTES(sp) # separately handled
  csrw mscratch, zero # indicate we are inside a trap handler
  csrr t0, mcause

  # -- Is this event an interrupt?
  bgez t0, .route_trap
  # -- -- Yes! Interrupt
  la ra, enclave_interrupt_table
  j .route_all_events

  # -- -- No, this is an OS trap
.route_trap:
  la ra, enclave_trap_table

.route_all_events:
  # Look up the handler address in the relevant event table
  # handler address = *( event_table_ptr + (mcause << 2) )
  sll t0, t0, 2
  add t0, ra, t0
  lwu ra, 0(t0) # load address of handler from event table

  # Jump to the event handler
  jr ra # route event
  # (no return)

# Reursive trap handler
# ---------------------

# Oh no! This trap came from within machine mode!
.recursive_trap_from_enclave:
  csrr sp, mscratch
  j .bad_event
# TODO: can we guarantee this is indeed a trap? mcause needs to not be out of range

# Calling convention helpers
# --------------------------

# TODO: turn this into a nice macro instead of a call maybe
.globl .enclave_save_regs_and_set_args
.enclave_save_regs_and_set_args:
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
.globl .enclave_restore_regs
.enclave_restore_regs:
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

# Generic event handlers
# ----------------------

.globl .enclave_perform_mret
.enclave_perform_mret:
  # Go back whence we came.
  ld t0, 2*REGBYTES(sp)
  csrw mscratch, t0
  
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0

  ld ra, 1*REGBYTES(sp)
  ld t0, 5*REGBYTES(sp)
  csrrw sp, mscratch, sp
  mret

.bad_event:
  csrr a0, mcause
  csrr a1, mepc
  csrr a2, mtval
  call .panic; // TODO: Implement real panic

.panic:
  j .panic
