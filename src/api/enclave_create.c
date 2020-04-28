#include <sm.h>
#include <stdint.h>
#include <stdbool.h>

api_result_t sm_internal_enclave_create (enclave_id_t enclave_id, uintptr_t ev_base, uintptr_t ev_mask, uint64_t num_mailboxes, bool debug) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a sequence of enclave_metadata_pages(num_mailboxes) metadata pages of type METADATA_FREE. The sequence must be contained within one region.
    - all configurations of ev_base, ev_mask (including invalid ones) are acceptable; these inputs are covered by measurement.
    - num_mailboxes affects the enclave metadata size, and is covered by measurement.
    - debug is covered by measurement.
      */

  region_map_t locked_regions = (const region_map_t){ 0 };

  uint64_t enclave_pages = enclave_metadata_pages(num_mailboxes);

  // <TRANSACTION>
  api_result_t result = add_lock_region_iff_free_metadata_pages( enclave_id, enclave_pages, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  uint64_t page_id = addr_to_region_page_id(enclave_id);
  uint64_t region_id = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);
  metadata_region_t * region = region_id_to_addr(region_id);

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Mark metadata pages as belonging to the enclave
  region->page_info[page_id] = METADATA_PAGE_ENCLAVE;
  for ( int i=1; i<enclave_pages; i++ ) {
    region->page_info[page_id+i] = METADATA_PAGE_INVALID;
  }

  // The selected pages were of type METADATA_PAGE_FREE --> are already zero.

  // Enclave initialization state
  enclave_metadata->init_state = ENCLAVE_STATE_CREATED;
  enclave_metadata->last_phys_addr_loaded = 0;
  hash_init( &enclave_metadata->hash_context );

  // Enclave parameters (covered by measurement)
  enclave_metadata->platform_csr.ev_base = ev_base;
  hash_extend(&enclave_metadata->hash_context, &enclave_metadata->platform_csr.ev_base, sizeof(enclave_metadata->platform_csr.ev_base));

  enclave_metadata->platform_csr.ev_mask = ev_mask;
  hash_extend(&enclave_metadata->hash_context, &enclave_metadata->platform_csr.ev_mask, sizeof(enclave_metadata->platform_csr.ev_mask));

  enclave_metadata->num_mailboxes = num_mailboxes;
  hash_extend(&enclave_metadata->hash_context, &enclave_metadata->num_mailboxes, sizeof(enclave_metadata->num_mailboxes));

  enclave_metadata->debug = (debug == true);
  hash_extend(&enclave_metadata->hash_context, &enclave_metadata->debug, sizeof(enclave_metadata->debug));

  // Enclave state
  // enclave_metadata->num_threads is zero
  // enclave_metadata->regions is a false array (zero)
  // enclave_metadata->mailboxes are zero (empty, not expecting mail)

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
