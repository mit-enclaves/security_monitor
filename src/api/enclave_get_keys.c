#include <sm.h>

api_result_t sm_internal_enclave_get_keys (
    uintptr_t addr_m,
    uintptr_t addr_pk,
    uintptr_t addr_sk,
    uintptr_t addr_s) {

  // Validate inputs
  // ---------------

  /*
     - the caller must be a valid enclave
     - the output buffers are guaranteed to belong to the caller because virtual memory access is done through the usual page walk with security checks being enforced
     */

  // Lock the enclave's metadata's region, the addr region and the os_addr region
  region_map_t locked_regions = (const region_map_t){ 0 };
  
  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

  // <TRANSACTION>
  // caller must be valid enclave
  api_result_t result = add_lock_region_iff_valid_enclave(caller, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy outputs
  memcpy_m2u((void *) addr_m, (void *) &(enclave_metadata->measurement), sizeof(hash_t));
  memcpy_m2u((void *) addr_pk, (void *) &(enclave_metadata->public_key), sizeof(public_key_t));
  memcpy_m2u((void *) addr_sk, (void *) &(enclave_metadata->secret_key), sizeof(secret_key_t));
  memcpy_m2u((void *) addr_s, (void *) &(enclave_metadata->attestation), sizeof(signature_t));

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
