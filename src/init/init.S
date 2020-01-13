#include <parameters.h>

.option norvc
.section .text.entry

# If (core id >= NUM_CORES), put the core to sleep.
# The SM cannot tolerate cores it wasn't configured to manage.

csrr t0, mhartid
li t0, NUM_CORES
TODO


# Scrub the core to pristine state
csrw mscratch, zero
li x1, 0
li x2, 0
call platform_clean_core
call platform_purge_core


# set stack pointer
la sp, stack_ptr
li t0, STACK_SIZE
csrr t1, mhartid
mul t0, t0, t1
sub sp, sp, t0    # sp = stack_ptr - (mhartid*STACK_SIZE)


# Run SM's high-level initialization routine
# call into sm_init()
call sm_init


# sm_init should not return --> panic if it does
li a0, 0x987329872
call platform_panic
