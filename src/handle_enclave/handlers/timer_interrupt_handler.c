#include <sm.h>
#include "../handles_enclave.h"
#include <stdint.h>

uint64_t timer_counter = 0;

void timer_interrupt_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc) {

  // Validate inputs
  // ---------------

  /*

  */

  // Get the core's lock
  // Get the enclave's metadata
  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  sm_state_t * sm = get_sm_state_ptr();

  uint64_t core_id = read_csr(mhartid);
  sm_core_t *core_metadata = &(sm->cores[core_id]);

  region_map_t locked_regions = (const region_map_t){ 0 };

  enclave_id_t enclave_id = core_metadata->owner;
  uint64_t region_id_enclave = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);
  
  // <TRANSACTION>
  // Lock the enclave's metadata region
  if(!add_lock_region(region_id_enclave, &locked_regions)) {
    sm_internal_enclave_aex(regs);
  }

  int64_t timer_limit = enclave_metadata->timer_limit;
  
  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  if(timer_counter > timer_limit) {
    sm_internal_enclave_aex(regs);
  }
  else {
    timer_counter++;
    return;
  }
}
