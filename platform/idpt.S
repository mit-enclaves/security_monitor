#include <sm.h>

.section ".rodata"
.globl idpt_ptr

#ifndef IDPT_FILE
  #define IDPT_FILE "idpt.bin"
#endif

.align PAGE_SHIFT

idpt:
.incbin IDPT_FILE
