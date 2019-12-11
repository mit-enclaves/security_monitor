#include <sm.h>

api_result_t sm_region_check_owned ( region_id_t region_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - the region must be in OWNED state
    - Selected region must be owned by the caller
  */

  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

  // <TRANSACTION>
  if ( !lock_region(region_id) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( sm->regions[region_id].state != REGION_STATE_OWNED ) {
    unlock_region( region_id );
    return MONITOR_INVALID_STATE;
  }

  if ( sm->regions[region_id].owner != caller ) {
    unlock_region( region_id );
    return MONITOR_ACCESS_DENIED;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // NOTE: this API call does not cause any state changes

  // Release locks
  unlock_region( region_id );
  // </TRANSACTION>

  return MONITOR_OK;
}
