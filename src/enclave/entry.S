#include <constants.h>

# TODO: Make sure stack never grows above 4KB !!

# Trap vector (table) for events originating in untrusted software
# ================================================================
  .section .sm.text.untrusted_trap
  .align 6 # Align to a cache line (64B) boundary
trap_table:
  # NOTE: only T0 are saved at this point
  .word .delegate_to_s          //  0: misaligned instruction address
  .word .pmp_trap               //  1: instruction access fault
  .word .illegal_insn_trap      //  2: illegal instruction
  .word .delegate_to_s          //  3: breakpoint
  .word .misaligned_load_trap   //  4: misaligned load addr
  .word .pmp_trap               //  5: load access fault
  .word .misaligned_store_trap  //  6: misaligned store/AMO addr
  .word .pmp_trap               //  7: store/AMO access fault
  .word .delegate_to_s          //  8: ECALL from U-mode
  .word .ecall_from_s_trap      //  9: ECALL from S-mode
  .word .bad_event              // 10: (reserved)
  .word .bad_event              // 11: ECALL from M-mode
  .word .delegate_to_s          // 12: instruction page fault
  .word .delegate_to_s          // 13: load page fault
  .word .bad_event              // 14: (reserved)
  .word .delegate_to_s          // 15: store/AMO page fault

  .align 6
interrupt_table:
  # NOTE: only T0 are saved at this point
  .word .delegate_to_s          //  0: user software interrupt
  .word .delegate_to_s          //  1: supervisor software interrupt
  .word .bad_event              //  2: (reserved)
  .word .handle_mipi            //  3: machine softwre interrupt
  .word .delegate_to_s          //  4: user timer interrupt
  .word .delegate_to_s          //  5: supervisor timer interrupt
  .word .bad_event              //  6: (reserved)
  .word .handle_mtimer_interrupt //  7: machine timer interrupt
  .word .bad_event              //  8: user external interrupt
  .word .bad_event              //  9: supervisor external interrupt
  .word .bad_event              // 10: (reserved)
  .word .bad_event              // 11: machine external interrupt

# Handler for events originating in untrusted software
# ====================================================
  .section .sm.text.untrusted_trap.entry
  .globl trap_vector_from_untrusted
trap_vector_from_untrusted:

  # 1. Prepare to route the event: set up an M-mode stack, make a0, a1 usable.
  csrrw sp, mscratch, sp # sp <--> mscratch (save SP, restore M-mode SP)
  beqz sp, .recursive_trap_from_untrusted
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
  la ra, interrupt_table
  j .route_all_events

  # -- -- No, this is an OS trap
.route_trap:
  la ra, trap_table

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
.recursive_trap_from_untrusted:
  csrr sp, mscratch
  j .bad_event

# TODO: can we guarantee this is indeed a trap? mcause needs to not be out of range

# Calling convention helpers
# --------------------------

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

# Generic event handlers
# ----------------------

.delegate_to_s:
  # delegate trap/interrupt to S mode
  # Set sIE to 0
  sd t1, 6*REGBYTES(sp)
  sd t2, 7*REGBYTES(sp)
  # Get MSTATUS
  csrr t0, mstatus
  # MSTATUS_SPP:    0x00000100
  # MSTATUS_SPIE:   0x00000020
  # MSTATUS_SIE:    0x00000002
  # MSTATUS_MPP:    0x00001800
  # sPIE <-- sIE
  andi t1, t0, 0x2
  sll t2, t1, 4
  # sPP <-- mPP[0]
  srl t1, t0, 3
  andi t1, t1, 0x100
  or t2, t2, t1
  # mPP <-- 0x1
  li t1, 0x0800
  or t2, t2, t1
  # Set sIE to 0, add sPP, sPIE
  li t1, 0xFFFFFFFFFFFFE6DD
  and t0, t0, t1
  or t0, t0, t2
  csrw mstatus, t0
  # sepc <-- mepc
  csrr t0, mepc
  csrw sepc, t0
  # scause <-- mcause
  csrr t0, mcause
  csrw scause, t0
  # sbadaddr <-- mbadaddr
  csrr t0, mbadaddr
  csrw sbadaddr, t0
  # Set M-mode return address to S-mode handler
  # mepc <-- stvec
  csrr t0, stvec
  csrw mepc, t0
  # Restore dirty registers and stack pointer
  ld t1, 6*REGBYTES(sp)
  ld t2, 7*REGBYTES(sp)
  # (continue to perform_mret)
.globl .perform_mret
.perform_mret:
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
  call .panic; // TODO: Implement real panic

.panic:
  j .panic
