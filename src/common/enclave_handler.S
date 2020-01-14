#include <sm_constants.h>

#ifndef ENCLAVE_HANDLER_FILE
  #error sm.enclave.bin path not defined
#endif

.section .rodata.enclave_handler

.globl enclave_handler_start, enclave_handler_end
enclave_handler_start:
  .incbin ENCLAVE_HANDLER_FILE
enclave_handler_end:
