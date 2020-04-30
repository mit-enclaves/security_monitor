#include "handles_enclave.h"
#include <sm.h>

void force_exit_enclave(api_result_t ret) {
  // TODO find a better solution for this
  platform_panic();
}

void sm_internal_enclave_aex (uintptr_t *regs) {
  
  // Validate inputs
  // ---------------

  /*
  
  */

  // Get the current thread's metadata
  // Lock the enclave and thread's metadata region (if different)
  // Save enclave state and signal AEX happend
  // Perform an enclave exit

  sm_state_t * sm = get_sm_state_ptr();

  uint64_t core_id = read_csr(mhartid);
  sm_core_t *core_metadata = &(sm->cores[core_id]);

  region_map_t locked_regions = (const region_map_t){ 0 };

  thread_id_t thread_id = core_metadata->thread;
  uint64_t region_id_thread = addr_to_region_id(thread_id);
  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;

  // <TRANSACTION>
  // Lock the thread's metadata region
  if(!add_lock_region(region_id_thread, &locked_regions)) {
    force_exit_enclave(MONITOR_CONCURRENT_CALL);
  }

  // Save enclave state
  for(int i = 0; i < NUM_REGISTERS; i++) {
    thread_metadata->aex_state[i] = regs[i];
  }
  
  // TODO: AEX overwrite previous AEX states, is that ok?

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  api_result_t ret = sm_internal_perform_enclave_exit(true);

  // Should not get there
  force_exit_enclave(ret);
}
