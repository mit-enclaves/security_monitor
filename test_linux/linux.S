#include <parameters.h>

.section .linux , "ax", %progbits

#ifndef LINUX_FILE
  #error linux.bin path not defined
#endif

.align PAGE_SHIFT

.globl kernel_start
kernel_start:
  .incbin LINUX_FILE
