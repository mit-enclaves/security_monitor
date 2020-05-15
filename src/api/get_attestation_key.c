#include <sm.h>

api_result_t sm_internal_get_attestation_key ( phys_ptr_t out_buffer ) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - caller must be a valid enclave
    - the caller's measurement must equal the SM's attestation enclave
    - the output buffer must fit entirely in one region
    - the output buffer region must belong to the caller
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  size_t buffer_size = sizeof(secret_key_t);

  uint64_t region_id = addr_to_region_id(out_buffer);
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id != addr_to_region_id(out_buffer+buffer_size-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t * sm = get_sm_state_ptr();
  sm_region_t * region_metadata = &sm->regions[region_id];
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

  // <TRANSACTION>
  if ( !add_lock_region(addr_to_region_id(caller), &locked_regions)) {
    return MONITOR_CONCURRENT_CALL;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);
  if ( 0 != memncmp(&enclave_metadata->measurement, &sm->signing_enclave_measurement, sizeof(hash_t) ) ) {
    return MONITOR_ACCESS_DENIED;
  }

  if ( !add_lock_region(region_id, &locked_regions) ) {
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  if ( region_metadata->owner != caller ) {
    unlock_regions(&locked_regions);
    return MONITOR_ACCESS_DENIED;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy the secret attestation key into the specified buffer
  sm_keys_t * keys = &sm->keys;
  memcpy( (uint8_t *)out_buffer, &keys->software_secret_key, buffer_size );

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
