#include <sm.h>

api_result_t sm_enclave_delete (enclave_id_t enclave_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
      - the enclave has no threads
      - all of the enclave's regions can be locked.
  */

  // <TRANSACTION>
  api_result_t result = lock_region_iff_valid_metadata( enclave_id, METADATA_PAGE_ENCLAVE );
  if ( MONITOR_OK != result ) {
    return result;
  }

  sm_state * sm = get_sm_state_ptr();
  uint64_t page_id = addr_to_region_page_id(enclave_id);
  uint64_t region_id = addr_to_region_id(enclave_id);
  enclave_t * enclave_metadata = (enclave_t *)(enclave_id);
  sm_region_t * region_metadata = &sm->regions[region_id];
  metadata_region_t * region = region_id_to_addr(region_id);
  region_map_t locked_regions = (const region_map_t){ 0 };
  locked_regions[region_id] = true;

  // Fail if the enclave has any threads associated with it
  if( enclave_metadata->num_threads > 0 ) {
    unlock_region( region_id );
    return MONITOR_INVALID_STATE;
  }

  // Fail if one of the enclave's regions cannot be locked
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    if ( enclave_metadata->regions[i] == true ) {
      if ( !lock_region(i) ) {
        unlock_regions( &locked_regions );
        return MONITOR_CONCURRENT_CALL;
      } else {
        locked_regions[i] = true;
      }
    }
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Block and erase all regions belonging to the enclave (these are already locked above)
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    if ( enclave_metadata->regions[i] == true ) {
      // Block the region
      sm->regions[i].state = REGION_STATE_BLOCKED;

      // Erase the region
      memset( region_id_to_addr(i), 0x00, REGION_SIZE);
    }
  }

  // Clean enclave metadata page map
  uint64_t enclave_pages = sm_enclave_metadata_pages(enclave_metadata->num_mailboxes);
  for ( i=0; i<enclave_pages; i++ ) {
    region->page_map[i+page_id] = METADATA_PAGE_FREE;
  }

  // Clean enclave metadata
  memset(enclave_metadata, 0x0, enclave_pages*PAGE_SIZE);

  // Release locks
  unlock_regions( &locked_regions );
  // </TRANSACTION>

  return MONITOR_OK;
}
