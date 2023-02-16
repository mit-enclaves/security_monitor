#include <sm.h>

api_result_t sm_internal_enclave_get_attest (enclave_id_t enclave_id, uintptr_t addr_m, uintptr_t addr_pk, uintptr_t addr_s) {

  // Validate inputs
  // ---------------

  /*
     - enclave_id must be valid
     - enclave must be in state ENCLAVE_STATE_INITIALIZED
     */

  // Lock the enclave's metadata's region, the phys_addr region and the os_addr region
  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  // enclave_id must be valid
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // enclave must be in state ENCLAVE_STATE_INITIALIZED
  if(enclave_metadata->init_state != ENCLAVE_STATE_INITIALIZED) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy outputs
  memcpy_m2u((void *) addr_m, (void *) &(enclave_metadata->measurement), sizeof(hash_t));
  memcpy_m2u((void *) addr_pk, (void *) &(enclave_metadata->public_key), sizeof(public_key_t));
  memcpy_m2u((void *) addr_s, (void *) &(enclave_metadata->attestation), sizeof(signature_t));

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
