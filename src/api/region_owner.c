#include <sm.h>

enclave_id_t sm_internal_region_owner (region_id_t region_id) {
  return region_owner(region_id);
}

enclave_id_t region_owner (region_id_t region_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
  */

  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];

  // <TRANSACTION>
  if ( !lock_region(region_id) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // NOTE: this API call does not cause any state changes

  enclave_id_t region_owner = region_metadata->owner;

  // Release locks
  unlock_region( region_id );
  // </TRANSACTION>

  return region_owner;
}

enclave_id_t region_owner_lock_free (region_id_t region_id) {
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];

  enclave_id_t region_owner = region_metadata->owner;

  return region_owner;
}
