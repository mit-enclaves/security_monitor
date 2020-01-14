#include <sm_constants.h>

#if (NUM_CORES != 1)
  #error platform_interrupt_other_cores is written for one core only
#endif

.option norvc
.section .text.platform_interrupt_other_cores

# void platform_interrupt_other_cores ();
.globl platform_interrupt_other_cores
platform_interrupt_other_cores:
  # TODO enable IPI; while true { wfi; }, IPI handler
  ret
