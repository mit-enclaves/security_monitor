#include <sm_constants.h>

.option norvc
.section .text.platform_panic

# void platform_panic (uint64_t error_code) __attribute__((noreturn));
.globl platform_panic
platform_panic:
  j platform_panic
