#include <sm.h>

api_result_t sm_internal_enclave_delete (enclave_id_t enclave_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
      - the enclave has no threads
      - all of the enclave's regions can be locked.
    - we need to lock the untrusted state and the core state
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  sm_state_t * sm = get_sm_state_ptr();
  uint64_t page_id = addr_to_region_page_id(enclave_id);
  uint64_t region_id = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);
  metadata_region_t * region = region_id_to_addr(region_id);


  // Fail if the enclave has any threads associated with it
  if( enclave_metadata->num_threads > 0 ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // Fail if one of the enclave's regions cannot be locked
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    if ( enclave_metadata->regions.flags[i] == true ) {
      if ( !lock_region(i) ) {
        unlock_regions(&locked_regions);
        return MONITOR_CONCURRENT_CALL;
      } else {
        locked_regions.flags[i] = true;
      }
    }
  }

  if(!lock_untrusted_state()) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE; 
  }

  int core_id = platform_get_core_id();

  if(!lock_core(core_id)) {
    unlock_untrusted_state();
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE; 
  }


  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Block all regions belonging to the enclave.
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    if ( enclave_metadata->regions.flags[i] == true ) {
      // Block the region
      sm->regions[i].state = REGION_STATE_BLOCKED;
      sm->regions[i].owner = OWNER_SM;
      
      // Clean the regions.
      memset( region_id_to_addr(region_id), 0x00, REGION_SIZE);
    }
  }

  // Clean enclave metadata page map
  uint64_t enclave_pages = enclave_metadata_pages(enclave_metadata->num_mailboxes);
  for ( int i=0; i<enclave_pages; i++ ) {
    region->page_info[i+page_id] = METADATA_PAGE_FREE;
  }

  // Clean enclave metadata
  memset(enclave_metadata, 0x0, enclave_pages*PAGE_SIZE);

  // Release locks
  unlock_core(core_id);
  unlock_untrusted_state();
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
