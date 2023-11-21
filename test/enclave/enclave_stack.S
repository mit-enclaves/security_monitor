#include <parameters.h>

.section ".data.stack"
.globl enclave_stack_base, enclave_stack_ptr
enclave_stack_base:
  .skip (STACK_SIZE)
enclave_stack_ptr:
