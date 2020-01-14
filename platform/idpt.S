#include <sm_constants.h>

.section ".rodata"
.globl idpt_ptr

#ifndef IDPT_FILE
  #error idpt.bin path not defined
#endif

.align PAGE_SHIFT
idpt:
  .incbin IDPT_FILE
