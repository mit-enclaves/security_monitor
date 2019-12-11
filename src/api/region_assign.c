#include <sm.h>

api_result_t sm_region_assign ( uint64_t region_id, enclave_id_t new_owner) {

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

  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];
  region_type_t =
  // <TRANSACTION>
  if ( !lock_region(region_id) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_FREE ) {
    unlock_region( region_id );
    return MONITOR_INVALID_STATE;
  }

  if ( new_owner == OWNER_UNTRUSTED ) {
    if ( !lock_untrusted_region_map() ) {
      unlock_region( region_id );
      return MONITOR_CONCURRENT_CALL;
    }

  } else {
    api_result_t result = lock_region_iff_valid_metadata( enclave_id, METADATA_PAGE_ENCLAVE );
    if ( MONITOR_OK != result ) {
      unlock_region( region_id );
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
    sm->untrusted_regions[region_id] = true;
  } else {
    enclave_t * enclave_metadata = (enclave_t *)(enclave_id);
    enclave_metadata->regions[region_id] = true;
  }

  // Release locks
  if ( new_owner == OWNER_UNTRUSTED ) {
    unlock_untrusted_region_map();
  } else {
    unlock_region( addr_to_region_id(new_owner) );
  }

  unlock_region( region_id );
  // </TRANSACTION>

  return MONITOR_OK;
}
