#include <sm.h>
#include "../handles_enclave.h"

void timer_interrupt_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc) {

  // Validate inputs
  // ---------------

  /*

  */

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  sm_internal_enclave_aex(regs);
}
