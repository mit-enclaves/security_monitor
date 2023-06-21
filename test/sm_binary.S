
#ifndef SM_BINARY_FILE
  #error sm.bin path not defined
#endif

.section .sm, "ax", %progbits

.globl sm_start, sm_end
sm_start:
  .incbin SM_BINARY_FILE
sm_end:
