#include <sm.h>

api_result_t sm_internal_region_free ( region_id_t region_id ) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - the region must be in BLOCKED state
    - the region should not be accessible by any core
    - the owner must be either OWNER_UNTRUSTED or a valid enclave
      - if OWNER_UNTRUSTED, the untrusted region map must be lockable
      - if enclave, the enclave metadata region must be lockable
    - we need to lock the untrusted state and the core state
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  if (!is_valid_region_id(region_id)) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];

  // <TRANSACTION>
  if (!add_lock_region(region_id, &locked_regions)) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_BLOCKED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // Check that the region is not accessible by any core
  for ( int i=0; i<NUM_CORES; i++ ) {
    if(!lock_core(i)) {
      unlock_regions(&locked_regions);
      return MONITOR_CONCURRENT_CALL;
    }
    if(region_is_accessible(sm->cores[i].mmrbm, region_id) 
        || region_is_accessible(sm->cores[i].memrbm, region_id)) {
      unlock_core(i);
      unlock_regions(&locked_regions);
      return MONITOR_INVALID_STATE;
    }
    unlock_core(i);
  }

  // TODO: Check that the region is not accessible by any core and get core locks.

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------
  
  // Flush the LLC region
  flush_llc_region(region_id);

  // Mark the selected region as free
  region_metadata->state = REGION_STATE_FREE;

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
