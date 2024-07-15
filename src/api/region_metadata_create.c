#include <sm.h>

api_result_t sm_internal_region_metadata_create (region_id_t region_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - region_id must be valid
    - the region must be in FREE state
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

  if ( region_metadata->state != REGION_STATE_FREE ) {
    unlock_region(region_id);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------
  metadata_region_t * metadata_region = region_id_to_addr(region_id);

  // Erase the metadata region - we may erase regions during free() instead, which departs a bit from the Sanctum paper
  memset( metadata_region, 0x00, REGION_SIZE );

  // Initialize the page_info structure
  int s = get_metadata_start_page();
  for (int i=0; i<NUM_REGION_PAGES; i++) {
    metadata_region->page_info[i] = (i<s) ? METADATA_PAGE_INVALID : METADATA_PAGE_FREE;
  }

  // Update region metadata
  region_metadata->owner = OWNER_SM;
  region_metadata->type = REGION_TYPE_METADATA;
  region_metadata->state = REGION_STATE_OWNED;

  // Release locks
  unlock_region( region_id );
  // </TRANSACTION>

  return MONITOR_OK;
}
