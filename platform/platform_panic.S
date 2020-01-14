#include <sm_constants.h>

#if (NUM_CORES != 1)
  #error platform_panic is written for one core only
#endif

.option norvc
.section .text

# void platform_panic (uint64_t error_code) __attribute__((noreturn));
.globl platform_panic
platform_panic:
  # TODO cause all other cores to panic also if NUM_CORES > 1
  j platform_panic
