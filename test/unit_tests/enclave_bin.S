#ifndef ENCLAVE_BINARY_FILE
  #error sm.bin path not defined
#endif

.section .enclave, "ax", %progbits

.globl enclave_start, enclave_end
enclave_start:
  .incbin ENCLAVE_BINARY_FILE
enclave_end:
