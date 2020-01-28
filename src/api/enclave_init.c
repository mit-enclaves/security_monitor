#include <sm.h>

api_result_t sm_internal_enclave_init (enclave_id_t enclave_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
      - the enclave has had its handler, page tables, and at least 1 page of data loded.
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // Make sure the enclave is in correct state:
  if (enclave_metadata->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Finalize enclave measurement
  hash_finalize( &enclave_metadata->hash_context, &enclave_metadata->measurement );

  // Initialize Enclave
  enclave_metadata->init_state = ENCLAVE_STATE_INITIALIZED;

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
