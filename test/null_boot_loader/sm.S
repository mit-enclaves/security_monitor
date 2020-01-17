#ifndef SM_BINARY_FILE
  #error sm.bin path not defined
#endif

.section .sm

.globl sm_start, sm_end
sm_start:
  .incbin ENCLAVE_HANDLER_FILE
sm_end:
