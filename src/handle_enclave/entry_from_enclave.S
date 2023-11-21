#include <sm_constants.h>

# TODO: Make sure stack never grows above 4KB !!
# ^ Do this via static analysis of the program

# TODO: add emulation for misaligned accesses, etc.

# Trap vector (table) for events originating from enclave
# ================================================================

.section .rodata.trap_table
.align 6 # Align to a cache line (64B) boundary
enclave_trap_table:
.t_tab_root:
  # NOTE: only T0 are saved at this point
  .word (.bad_event-.t_tab_root)          //  0: misaligned instruction address
  .word (.bad_event-.t_tab_root)          //  1: instruction access fault
  .word (.bad_event-.t_tab_root)          //  2: illegal instruction
  .word (.bad_event-.t_tab_root)          //  3: breakpoint
  .word (.bad_event-.t_tab_root)          //  4: misaligned load addr
  .word (.bad_event-.t_tab_root)          //  5: load access fault
  .word (.bad_event-.t_tab_root)          //  6: misaligned store/AMO addr
  .word (.bad_event-.t_tab_root)          //  7: store/AMO access fault
  .word (.ecall_from_e_trap-.t_tab_root)  //  8: ECALL from U-mode
  .word (.ecall_from_e_trap-.t_tab_root)  //  9: ECALL from S-mode
  .word (.bad_event-.t_tab_root)          // 10: (reserved)
  .word (.bad_event-.t_tab_root)          // 11: ECALL from M-mode
  .word (.bad_event-.t_tab_root)          // 12: instruction page fault
  .word (.bad_event-.t_tab_root)          // 13: load page fault
  .word (.bad_event-.t_tab_root)          // 14: (reserved)
  .word (.bad_event-.t_tab_root)          // 15: store/AMO page fault

.align 6 # Align to a cache line (64B) boundary
enclave_interrupt_table:
.i_tab_root:
  # NOTE: only T0 are saved at this point
  .word (.bad_event-.i_tab_root)          //  0: user software interrupt
  .word (.bad_event-.i_tab_root)          //  1: supervisor software interrupt
  .word (.bad_event-.i_tab_root)          //  2: (reserved)
  .word (.sw_int_handler-.i_tab_root)          //  3: machine softwre interrupt
  .word (.bad_event-.i_tab_root)          //  4: user timer interrupt
  .word (.bad_event-.i_tab_root)          //  5: supervisor timer interrupt
  .word (.bad_event-.i_tab_root)          //  6: (reserved)
  .word (.timer_int_handler-.i_tab_root)          //  7: machine timer interrupt
  .word (.bad_event-.i_tab_root)          //  8: user external interrupt
  .word (.bad_event-.i_tab_root)          //  9: supervisor external interrupt
  .word (.bad_event-.i_tab_root)          // 10: (reserved)
  .word (.ext_int_handler-.i_tab_root)          // 11: machine external interrupt

# Handler for events originating from an enclave
# ====================================================

.section .text.entry
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

  # Deactivate Speculation and Flush Microarchitecture
  li t0, MSPEC_NONE
  csrw CSR_MSPEC, MSPEC_NONMEM
  csrw CSR_MFLUSH, zero

  # Copy mcause to t0
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
  lw t0, 0(t0) # load *offset* of handler from event table, sign extend
  add ra, ra, t0

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
.globl .ecall_from_e_trap
.ecall_from_e_trap:
  call .save_regs_and_set_args
  call ecall_from_enclave_trap
  call .restore_regs
  j .perform_mret_ecall

# Software interrupt
.globl .sw_int_handler
.sw_int_handler:
  call .save_regs_and_set_args
  call enclave_software_interrupt_handler
  call .restore_regs
  j .perform_mret

# Timer interrupt
.globl .timer_int_handler
.timer_int_handler:
  call .save_regs_and_set_args
  call timer_interrupt_handler
  call .restore_regs
  j .perform_mret

# External interrupt
.globl .ext_int_handler
.ext_int_handler:
  call .save_regs_and_set_args
  call external_interrupt_handler
  call .restore_regs
  j .perform_mret

# Generic event handlers
# ----------------------

.bad_event:
#if (DEBUG_ENCLAVE == 1)
  csrr a0, mcause
  csrr a1, mepc
  csrr a2, mtval
  call bad_trap
#else
  call platform_panic
#endif
