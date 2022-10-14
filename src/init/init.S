#include <parameters.h>

.option norvc
.section .text.entry

# Scrub the core to pristine state
csrw mscratch, zero
li x1, 0
mv x2, a1 # Save the device tree address on x2
call platform_clean_core
call platform_purge_core

# Set up argument for future sm_init call
mv a0, x2
li x2, 0 # Clean x2

# set stack pointer
la sp, stack_ptr
li t0, STACK_SIZE
csrr t1, mhartid
mul t0, t0, t1
li t1, MENTRY_FRAME_SIZE
sub sp, sp, t0    # sp = stack_ptr - (mhartid*STACK_SIZE)
sub sp, sp, t1    # sp = stack_ptr - (mhartid*STACK_SIZE) - MENTRY_FRAME_SIZE

# Run SM's high-level initialization routine; safe for all cores to run
# call into sm_init(uintptr_t fdt_addr)
call sm_init

# sm_init should not return --> panic if it does
li a0, 0x987329872
call platform_panic
