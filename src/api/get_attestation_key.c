#include <sm.h>

api_result_t sm_get_attestation_key ( uintptr_t out_buffer ) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - caller must be a valid enclave
    - the caller's measurement must equal the SM's attestation enclave
    - the output buffer must fit entirely in one region
    - the output buffer region must belong to the caller
  */

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
  enclave_id_t caller = sm->cores[get_core_id()].owner;

  // <TRANSACTION>
  if ( !lock_region(addr_to_region_id(caller)) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  enclave_t * enclave_metadata = (enclave_t *)(caller);
  if ( 0 != memcmp(&enclave_metadata->measurement, &sm->signing_enclave_measurement, sizeof(hash_t) ) ) {
    return MONITOR_ACCESS_DENIED;
  }

  if ( !lock_region(region_id) ) {
    unlock_region( addr_to_region_id(caller) );
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_OWNED ) {
    unlock_region( region_id );
    unlock_region( addr_to_region_id(caller) );
    return MONITOR_INVALID_STATE;
  }

  if ( region_metadata->owner != caller ) {
    unlock_region( region_id );
    unlock_region( addr_to_region_id(caller) );
    return MONITOR_ACCESS_DENIED;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy the secret attestation key into the specified buffer
  sm_keys_t * keys = get_sm_keys_ptr();
  memcpy( (uint8_t *)out_buffer, &keys->software_secret_key, buffer_size );

  // Release locks
  unlock_region( region_id );
  unlock_region( addr_to_region_id(caller) );
  // </TRANSACTION>

  return MONITOR_OK;
}
