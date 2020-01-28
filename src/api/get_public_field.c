#include <sm.h>

api_result_t sm_internal_get_public_field (public_field_t field, phys_ptr_t out_buffer) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - field must be one of the fields implemented
    - the output buffer must fit entirely in one region
    - the output buffer region must belong to the caller
  */



  sm_state_t * sm = get_sm_state_ptr();
  sm_keys_t * keys = &sm->keys;
  size_t buffer_size = 0;
  uint8_t * buffer_src;

  switch (field) {
    case PUBLIC_FIELD_PK_M:
      buffer_size = sizeof(keys->manufacturer_public_key);
      buffer_src = keys->manufacturer_public_key.bytes;
      break;

    case PUBLIC_FIELD_PK_D:
      buffer_size = sizeof(keys->device_public_key);
      buffer_src = keys->device_public_key.bytes;
      break;

    case PUBLIC_FIELD_PK_SM:
      buffer_size = sizeof(keys->software_public_key);
      buffer_src = keys->software_public_key.bytes;
      break;

    case PUBLIC_FIELD_H_SM:
      buffer_size = sizeof(hash_t);
      buffer_src = keys->software_measurement.bytes;
      break;

    case PUBLIC_FIELD_SIG_M:
      buffer_size = sizeof(keys->device_signature);
      buffer_src = keys->device_signature.bytes;
      break;

    case PUBLIC_FIELD_SIG_D:
      buffer_size = sizeof(keys->software_signature);
      buffer_src = keys->software_signature.bytes;
      break;

    case PUBLIC_FIELD_H_AE:
      buffer_size = sizeof(sm->signing_enclave_measurement);
      buffer_src = sm->signing_enclave_measurement.bytes;
      break;

    default:
      return MONITOR_INVALID_VALUE;
  }

  uint64_t region_id = addr_to_region_id(out_buffer);
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id != addr_to_region_id(out_buffer+buffer_size-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  sm_region_t * region_metadata = &sm->regions[region_id];
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

  // <TRANSACTION>
  if ( !lock_region(region_id) ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( region_metadata->state != REGION_STATE_OWNED ) {
    unlock_region( region_id );
    return MONITOR_INVALID_STATE;
  }

  if ( region_metadata->owner != caller ) {
    unlock_region( region_id );
    return MONITOR_ACCESS_DENIED;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy the requested field into the specified buffer
  memcpy( (uint8_t *)out_buffer, buffer_src, buffer_size );

  // Release locks
  unlock_region( region_id );
  // </TRANSACTION>

  return MONITOR_OK;
}
