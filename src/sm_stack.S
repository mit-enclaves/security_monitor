.section ".sm_stacks"
.globl stack_base, stack_ptr
.align 12 // PAGE_SHIFT
stack_base:
.skip 0x2000 // SIZE_PAGE * NUM_CORES
stack_ptr:
