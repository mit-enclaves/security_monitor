#include <sm.h>

api_result_t sm_internal_region_block ( region_id_t region_id ) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - the region must be in OWNED state
    - the region cannot be an SM region
    - Selected region must be owned by the caller
  */

  region_map_t locked_regions = (const region_map_t){ 0 };
  
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;
  int core_id = platform_get_core_id();

  // <TRANSACTION>
  if ( !add_lock_region(region_id, &locked_regions) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  if ( region_metadata->type == REGION_TYPE_SM ) {
    unlock_regions(&locked_regions);
    return MONITOR_ACCESS_DENIED;
  }

  if ( region_metadata->owner != caller ) {
    unlock_regions(&locked_regions);;
    return MONITOR_ACCESS_DENIED;
  }

  if (!lock_untrusted_state()) {
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }

  if (!lock_core(core_id)) {
    unlock_untrusted_state();
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }
  
  if ( region_metadata->owner == OWNER_SM ) {
    unlock_core(core_id);
    unlock_untrusted_state();
    unlock_regions(&locked_regions);
    return MONITOR_ACCESS_DENIED;
  } else if (region_metadata->owner != OWNER_UNTRUSTED) { // The owner is a valid enclave
    api_result_t result = add_lock_region_iff_valid_enclave(region_metadata->owner, &locked_regions);
    if (result != MONITOR_OK) {
      unlock_core(core_id);
      unlock_untrusted_state();
      unlock_regions(&locked_regions);
      return MONITOR_CONCURRENT_CALL;
    }
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Transfer ownership of the region
  enclave_id_t former_owner = region_metadata->owner;
  region_metadata->owner = OWNER_SM;

  // Remove the region from owner's region map
  if ((former_owner != OWNER_UNTRUSTED) && (former_owner != OWNER_SM)){
    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(former_owner);
    platform_update_enclave_regions(sm, core_id, enclave_metadata, region_id, false);
  }
  else if (former_owner == OWNER_UNTRUSTED) {
    int core_id = platform_get_core_id();
    platform_update_untrusted_regions(sm, core_id, region_id, false);
  }

 // Block the selected region
  region_metadata->state = REGION_STATE_BLOCKED;

  // Release locks
  unlock_core(core_id);
  unlock_untrusted_state();
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
