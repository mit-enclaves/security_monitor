#include <sm_constants.h>

# TODO: Make sure stack never grows above 4KB !!

# Trap vector (table) for events originating in untrusted software
# ================================================================
.section .rodata.trap_table
.align 6 # Align to a cache line (64B) boundary
trap_table:
.t_tab_root:
  # NOTE: only T0 are saved at this point
  .word (.delegate_to_s-.t_tab_root)      //  0: misaligned instruction address
  .word (.pmp_trap-.t_tab_root)           //  1: instruction access fault
  .word (.illegal_insn_trap-.t_tab_root)  //  2: illegal instruction
  .word (.delegate_to_s-.t_tab_root)      //  3: breakpoint
  .word (.misaligned_load_trap-.t_tab_root)   //  4: misaligned load addr
  .word (.pmp_trap-.t_tab_root)           //  5: load access fault
  .word (.misaligned_store_trap-.t_tab_root)  //  6: misaligned store/AMO addr
  .word (.pmp_trap-.t_tab_root)           //  7: store/AMO access fault
  .word (.delegate_to_s-.t_tab_root)      //  8: ECALL from U-mode
  .word (.ecall_from_s_trap-.t_tab_root)  //  9: ECALL from S-mode
  .word (.bad_event-.t_tab_root)          // 10: (reserved)
  .word (.bad_event-.t_tab_root)          // 11: ECALL from M-mode
  .word (.delegate_to_s-.t_tab_root)      // 12: instruction page fault
  .word (.delegate_to_s-.t_tab_root)      // 13: load page fault
  .word (.bad_event-.t_tab_root)          // 14: (reserved)
  .word (.delegate_to_s-.t_tab_root)      // 15: store/AMO page fault

.align 6 # Align to a cache line (64B) boundary
interrupt_table:
.i_tab_root:
  # NOTE: only T0 are saved at this point
  .word (.delegate_to_s-.i_tab_root)  //  0: user software interrupt
  .word (.delegate_to_s-.i_tab_root)  //  1: supervisor software interrupt
  .word (.bad_event-.i_tab_root)      //  2: (reserved)
  .word (.handle_m_soft_interrupt-.i_tab_root)  //  3: machine software interrupt
  .word (.delegate_to_s-.i_tab_root)  //  4: user timer interrupt
  .word (.delegate_to_s-.i_tab_root)  //  5: supervisor timer interrupt
  .word (.bad_event-.i_tab_root)      //  6: (reserved)
  .word (.handle_mtimer_interrupt-.i_tab_root) //  7: machine timer interrupt
  .word (.delegate_to_s-.i_tab_root)  //  8: user external interrupt
  .word (.delegate_to_s-.i_tab_root)  //  9: supervisor external interrupt
  .word (.bad_event-.i_tab_root)      // 10: (reserved)
  .word (.bad_event-.i_tab_root)  // 11: machine external interrupt

# Handler for events originating in untrusted software
# ====================================================

.section .text.trap_from_untrusted
.globl trap_vector_from_untrusted
trap_vector_from_untrusted:
  # 1. Prepare to route the event: set up an M-mode stack, make a0, a1 usable.
  csrrw sp, mscratch, sp # sp <--> mscratch (save SP, restore M-mode SP)
  beqz sp, .recursive_trap_from_untrusted
  # TODO: the above is problematic. Make sure recursion does not blow the stack.

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
  lw t0, 0(t0) # load *offset* of handler from event table, sign extend
  add ra, ra, t0

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
.globl .ecall_from_s_trap
.ecall_from_s_trap:
  call .save_regs_and_set_args
  call ecall_from_s_trap
  call .restore_regs
  j .perform_mret_ecall


# Generic event handlers
# ----------------------

.pmp_trap:
  j .bad_event

.illegal_insn_trap:
  call .save_regs_and_set_args
  call illegal_instruction_trap_handler
  call .restore_regs
  j .perform_mret_ecall

.misaligned_load_trap:
  j .bad_event

.misaligned_store_trap:
  j .bad_event

# Clear MTIE and rise STIP
# STIP are direclty forwarded to S mode
.handle_mtimer_interrupt:
  li t0, MIP_MTIP
  csrc mie, t0
  li t0, MIP_STIP
  csrs mip, t0
  j .perform_mret

.handle_m_soft_interrupt:
  # First, clear the MIPI bit.
  ld t0, MENTRY_IPI_OFFSET(sp)
  sw x0, (t0)
  fence

  # Now, decode the cause(s).
  addi t0, sp, MENTRY_IPI_PENDING_OFFSET
  amoswap.w t0, x0, (t0)
  call .save_regs_and_set_args
  mv a0, t0
  call untrusted_software_interrupt_handler
  call .restore_regs

  and t1, t0, IPI_SOFT
  beqz t1, 1f
  csrs mip, MIP_SSIP
  1:
  andi t1, t0, IPI_FENCE_I
  beqz t1, 1f
  fence.i
  1:
  andi t1, t0, IPI_SFENCE_VMA
  beqz t1, 1f
  sfence.vma
  1:
  andi t1, t0, IPI_HALT
  beqz t1, 1f
  wfi
  j 1b
  1:
  ld t1, 6*REGBYTES(sp)
  j .perform_mret

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
  # Restore dirty registers, return address register and stack pointer
  ld t1, 6*REGBYTES(sp)
  ld t2, 7*REGBYTES(sp)
  j .perform_mret

.bad_event:
  csrr a0, mcause
  csrr a1, mepc
  csrr a2, mtval
  call bad_trap
