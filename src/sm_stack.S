#include "machine.h"
#include "encoding.h"

.section ".stacks"
.globl stack_base, stack_ptr
.align RISCV_PGSHIFT
stack_base:
.skip RISCV_PGSIZE * NUM_CORES
stack_ptr:
