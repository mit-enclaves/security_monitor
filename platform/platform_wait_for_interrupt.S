#include <sm_constants.h>

#if (NUM_CORES != 1)
  #error platform_wait_for_interrupt is written for one core only
#endif

.option norvc
.section .text.platform_wait_for_interrupt

# void platform_wait_for_interrupt ();
.globl platform_wait_for_interrupt
platform_wait_for_interrupt:
  # TODO enable IPI; while true { wfi; }, IPI handler
  ret
