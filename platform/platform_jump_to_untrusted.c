#include <sm.h>

__attribute__((section(".text.platform_jump_to_untrusted")))

extern uint8_t trap_vector_from_untrusted;

void platform_jump_to_untrusted ( region_map_t * region_map, uint64_t virtual_pc, uint64_t virtual_sp ) __attribute__((noreturn));

void platform_jump_to_untrusted ( region_map_t * region_map, uint64_t virtual_pc, uint64_t virtual_sp ) {

  // Set the interrupt handler address ()
  write_csr(mtvec, ( ((uint64_t)(&trap_vector_from_untrusted))&(~0x3L) ));

  // Set up the privilege stack for an S-mode return
  uint64_t mstatus_csr = read_csr(mstatus);
  mstatus_csr &= ~((0x11<<11) | (0x1<<7)); // mask out MPP and MPIE
  mstatus_csr |= ((0x01<<11) | (0x1<<7)); // set MPP="S"=0x01 and MPIE=1
  write_csr(mstatus, mstatus_csr);

  register uint64_t t0 asm ("t0") = STACK_SIZE;
  register uintptr_t a0 asm ("a0") = virtual_pc;
  register uintptr_t a1 asm ("a1") = virtual_sp;
  asm volatile (" \
    # Set S-mode PC \n \
    csrw mepc, a0; \n \
    \
    # Set the core's SM stack pointer \n \
    la sp, stack_ptr; \n \
    csrr t1, mhartid; \n \
    mul t0, t0, t1; \n \
    sub sp, sp, t0; # sp = stack_ptr - (mhartid*STACK_SIZE) \n \
    csrw mscratch, sp; \n \
    \
    # Clean the cores; \n \
    call platform_clean_core; \n \
    call platform_purge_core; \n \
    mv sp, a1; \n \
    li ra, 0; \n \
    \
    mret " : : "r" (t0), "r" (a0), "r" (a1));

  __builtin_unreachable();
}
