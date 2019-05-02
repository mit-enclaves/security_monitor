#include <constants.h>

.option norvc
.section .sm.text.untrusted_trap

.globl .handle_mtimer_interrupt
.handle_mtimer_interrupt:
  #Simply clear MTIE and raise STIP.
  # TODO: shouldn't this actually delegate the interrupt to S-mode??
  li t0, MIP_MTIP
  csrc mie, t0
  li t0, MIP_STIP
  csrs mip, t0
  j .perform_mret
