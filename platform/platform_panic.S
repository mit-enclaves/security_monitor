#include <sm_constants.h>

.option norvc
.section .text.platform_panic

# void platform_panic (uint64_t error_code) __attribute__((noreturn));
.globl platform_panic
platform_panic:
  # TODO cause all other cores to panic also if NUM_CORES > 1
  j platform_panic
