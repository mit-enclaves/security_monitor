#include <sm.h>

.section ".data"
.globl stack_base, stack_ptr

.align PAGE_SIZE

stack_base:
.skip (STACK_SIZE*NUM_CORES)
stack_ptr:
