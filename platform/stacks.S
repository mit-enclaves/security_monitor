#include <sm_constants.h>

.section ".data"

.align PAGE_SHIFT

.globl stack_base, stack_ptr

stack_base:
  .skip (STACK_SIZE*NUM_CORES)
stack_ptr:
