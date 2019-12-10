#include <sm.h>

api_result_t sm_enclave_create (enclave_id_t enclave_id, uintptr_t ev_base, uintptr_t ev_mask, uint64_t num_mailboxes, bool debug) {
  // TODO: Check all arguments validity

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a sequence of sm_enclave_metadata_pages(mailbox_count) metadata pages of type METADATA_FREE. The sequence must be contained within one region.
    - all configurations of ev_base, ev_mask (including invalid ones) are acceptable; these inputs are covered by measurement.
    - mailbox_count affects the enclave metadata size, and is covered by measurement.
    - debug is covered by measurement.
      */

  // Perform validation that does not require a lock first.

  // Check that enclave_id is page alligned
  if ( !page_aligned(enclave_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Check that the requested region is indeed a region in RAM
  uint64_t region_id = addr_to_region_id(enclave_id);
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Check that the chosen enclave_id does not overlap with region page info structure.
  uint64_t page_id = addr_to_region_page_id(enclave_id);
  // region page info table is marked as METADATA_INVALID, so we do not need to explicitly check that page_id > sm_region_metadata_start().

  // Check that the enclave structure does not span regions
  uint64_t enclave_pages = sm_enclave_metadata_pages(mailbox_count);
  if ( is_valid_page_id_in_region(page_id+enclave_pages-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // A lock on the requested metadata region is required
  sm_state * sm = get_sm_state_ptr();
  // <TRANSACTION>
  {
    if ( !platform_lock_acquire( sm->regions[region_id].lock ) ) {
      return MONITOR_CONCURRENT_CALL;
    }

    // Check that the requested region is indeed a metadata region
    if ( sm->regions[region_id].type != REGION_TYPE_METADATA ) {
      platform_lock_release( sm->regions[region_id].lock );
      return MONITOR_INVALID_STATE;
    }

    // Check that each of the requested metadata pages is of type METADATA_FREE
    for ( int i=0; i<enclave_pages; i++ ) {
      if ( metadata_page_type( sm->regions[region_id].region->page_info[i] ) != METADATA_FREE; ) {
        platform_lock_release( sm->regions[region_id].lock );
        return MONITOR_INVALID_STATE;
      }
    }

    // NOTE: Inputs are now deemed valid.

    // Apply state transition
    // ----------------------

    // Mark metadata pages as belonging to the enclave
    sm->regions[region_id].region->page_info[0] = make_page_info(enclave_id, METADATA_ENCLAVE);
    for ( int i=1; i<enclave_pages; i++ ) {
      sm->regions[region_id].region->page_info[i] = make_page_info(enclave_id, METADATA_INVALID);
    }

    // The selected pages were of type METADATA_FREE --> are already zero.

    // Enclave initialization state
    enclave_id->init_state = ENCLAVE_STATE_CREATED;
    enclave_id->last_phys_addr_loaded = 0;
    hash_init( &enclave_id->hash_context );

    // Enclave parameters (covered by measurement)
    enclave_id->ev_base = ev_base;
    hash_extend(&enclave_id->hash_context, &enclave_id->ev_base, sizeof(enclave_id->ev_base));

    enclave_id->ev_base = ev_mask;
    hash_extend(&enclave_id->hash_context, &enclave_id->ev_mask, sizeof(enclave_id->ev_mask));

    enclave_id->num_mailboxes = num_mailboxes;
    hash_extend(&enclave_id->hash_context, &enclave_id->num_mailboxes, sizeof(enclave_id->num_mailboxes));

    enclave_id->debug = debug;
    hash_extend(&enclave_id->hash_context, &enclave_id->debug, sizeof(enclave_id->debug));

    // Enclave state
    // enclave_id->num_threads is zero
    // enclave_id->regions is a false array (zero)
    // enclave_id->mailboxes are zero (empty, not expecting mail)

    platform_lock_release( sm->regions[region_id].lock );
  }
  // </TRANSACTION>

   return monitor_ok;
}
