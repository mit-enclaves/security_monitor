#include <sm.h>

api_result_t sm_internal_region_free ( region_id_t region_id ) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - the region must be in BLOCKED state
    - the owner must be either OWNER_UNTRUSTED or a valid enclave
      - if OWNER_UNTRUSTED, the untrusted region map must be lockable
      - if enclave, the enclave metadata region must be lockable
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

  // NOTE: regions of type SM could not have been blocked. Such regions cannot be ever freed.
  if ( region_metadata->owner == OWNER_UNTRUSTED ) {
    if (!lock_untrusted_state()) {
      unlock_regions(&locked_regions);
      return MONITOR_CONCURRENT_CALL;
    }

  } else {
    // The owner is a valid enclave
    if (!add_lock_region(addr_to_region_id(region_metadata->owner), &locked_regions)) {
      unlock_regions(&locked_regions);
      return MONITOR_CONCURRENT_CALL;
    }

  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Transfer ownership of the region
  enclave_id_t former_owner = region_metadata->owner;
  region_metadata->owner = OWNER_UNTRUSTED;

  // Remove the region from owner's region map
  if ((former_owner != OWNER_UNTRUSTED) && (former_owner != OWNER_SM)){
    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(former_owner);
    platform_update_enclave_regions(enclave_metadata, region_id, false);
  }

  // Add the region to the untrusted region map
  platform_update_untrusted_regions(sm, region_id, true);

  if (CLEAN_REGIONS_MEMSET) {
    // NOTE: In Sanctum, freeing the region does *not* erase its contents - this is the enclave's responsibility, except when deleted. This implementation, however, does.
    memset( region_id_to_addr(region_id), 0x00, REGION_SIZE );

    // Else the enclave's regions are erased when they are blocked at enclave deletion, and the enclave is responsible for cleaning any regions it itself blocks.
  }

  // Mark the selected region as free
  region_metadata->state = REGION_STATE_FREE;

  // Release locks
  // (NOTE: owner field is conveniently not cleared until the region is re-assigned)
  if ( former_owner == OWNER_UNTRUSTED ) {
    unlock_untrusted_state();
  } else { // a valid enclave
    unlock_regions(&locked_regions);
  }

  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
