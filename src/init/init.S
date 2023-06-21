#include <parameters.h>

.option norvc
.section .text.entry

# Deactivate speculation
li t0, MSPEC_NONE
csrw CSR_MSPEC, MSPEC_NONMEM

# Scrub the core to pristine state
csrw mscratch, zero
li x1, 0
mv x2, a1 # Save the device tree address on x2
call platform_clean_core
call platform_purge_core

# Set up argument for future sm_init call
mv a0, x2
mv x2, zero # Clean x2

# set stack pointer
la sp, stack_ptr
li t0, STACK_SIZE
csrr t1, mhartid
mul t0, t0, t1
li t1, MENTRY_FRAME_SIZE
sub sp, sp, t0    # sp = stack_ptr - (mhartid*STACK_SIZE)
sub sp, sp, t1    # sp = stack_ptr - (mhartid*STACK_SIZE) - MENTRY_FRAME_SIZE

# core 0 zero out bss region
csrr t0, mhartid
bne t0, zero, 2f
la t0, _bss_start
la t1, _bss_end
beq t0, t1, 2f
1:sd zero, 0(t0)
addi t0, t0, REGBYTES
bne t0, t1, 1b
2:

mv t0, zero
mv t1, zero

# Run SM's high-level initialization routine; safe for all cores to run
# call into sm_init(uintptr_t fdt_addr)
call sm_init

# sm_init should not return --> panic if it does
li a0, 0x987329872
call platform_panic
