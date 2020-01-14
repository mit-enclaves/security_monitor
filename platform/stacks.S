#include <sm_constants.h>

.section ".data"
.globl stack_base, stack_ptr

.align PAGE_SHIFT
stack_base:
.skip (STACK_SIZE*NUM_CORES)
stack_ptr:
