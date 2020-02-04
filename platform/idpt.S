#include <parameters.h>

.section .idpt , "ax", %progbits

#ifndef IDPT_FILE
  #error idpt.bin path not defined
#endif

.align PAGE_SHIFT

.globl platform_idpt
platform_idpt:
  .incbin IDPT_FILE
