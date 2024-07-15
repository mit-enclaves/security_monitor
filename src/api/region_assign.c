#include <sm.h>

api_result_t sm_internal_region_assign ( region_id_t region_id, enclave_id_t new_owner) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - selected region must be in REGION_STATE_FREE state
    - new_owner must be OWNER_UNTRUSTED or a valid enclave
      - if the new owner is OWNER_UNTRUSTED, its region map must be lockable
      - if the new owner is a valid enclave, it must be lockable
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];
  int core_id = platform_get_core_id();

  // <TRANSACTION>
  if (!add_lock_region(region_id, &locked_regions) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_FREE ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  if ( !lock_untrusted_state() ) {
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }

  if ( !lock_core(core_id) ) {
    unlock_untrusted_state();
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }

  if(new_owner != OWNER_UNTRUSTED) {
    api_result_t result = add_lock_region_iff_valid_enclave(new_owner, &locked_regions);
    if ( MONITOR_OK != result ) {
      unlock_regions(&locked_regions);
      return result;
    }
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Transfer ownership of the region
  region_metadata->owner = new_owner;
  region_metadata->type = (new_owner==OWNER_UNTRUSTED) ? REGION_TYPE_UNTRUSTED : REGION_TYPE_ENCLAVE;
  region_metadata->state = REGION_STATE_OWNED;

  // Mark the newly gained region in the new owner's region map
  if ( new_owner == OWNER_UNTRUSTED ) {
    platform_update_untrusted_regions(sm, core_id, region_id, true);
  } else {
    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(new_owner);
    platform_update_enclave_regions(sm, core_id, enclave_metadata, region_id, true);
  }

  // Release locks
  unlock_core(core_id);
  unlock_untrusted_state();
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
