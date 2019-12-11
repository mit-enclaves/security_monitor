#include <sm.h>

api_result_t sm_enclave_init (enclave_id_t enclave_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
      - the enclave has had its handler, page tables, and at least 1 page of data loded.
  */

  // <TRANSACTION>
  api_result_t result = lock_region_iff_valid_metadata( enclave_id, METADATA_PAGE_ENCLAVE );
  if ( MONITOR_OK != result ) {
    return result;
  }

  sm_state * sm = get_sm_state_ptr();
  uint64_t region_id = addr_to_region_id(enclave_id);
  enclave_t * enclave_metadata = (enclave_t *)(enclave_id);

  // Make sure the enclave is in correct state:
  if (enclave->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED) {
    unlock_region( region_id );
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Finalize enclave measurement
  hash_finalize( &enclave_metadata->hash_context, &enclave_metadata->measurement );

  // Initialize Enclave
  enclave->init_state = ENCLAVE_STATE_INITIALIZED;

  // Release locks
  unlock_region( region_id );
  // </TRANSACTION>

  return monitor_ok;
}
