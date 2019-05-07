.globl os_stack_base, os_stack_ptr
.section ".os.stack"
.align 12 // PAGE_SHIFT
os_stack_base:
.skip 0x2000 // SIZE_PAGE * NUM_CORES
os_stack_ptr:
