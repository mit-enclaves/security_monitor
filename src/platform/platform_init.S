.option norvc
.section .text.entry

# assert mhartid < NUM_CORES else panic
# TODO

#assert stack_ptr is page aligned else panic
# TODO

# erase core
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

# call into sm_init()
call sm_init

# sm_init should not return --> panic if it does
call enclave_platform_panic
