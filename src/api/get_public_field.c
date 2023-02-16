#include <sm.h>

api_result_t sm_internal_get_public_field (public_field_t field, uintptr_t out_buffer) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - field must be one of the fields implemented
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

    case PUBLIC_FIELD_SIG_D:
      buffer_size = sizeof(keys->device_signature);
      buffer_src = keys->device_signature.bytes;
      break;

    case PUBLIC_FIELD_SIG_SM:
      buffer_size = sizeof(keys->software_signature);
      buffer_src = keys->software_signature.bytes;
      break;

    default:
      return MONITOR_INVALID_VALUE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy the requested field into the specified buffer
  memcpy_m2u( (uint8_t *)out_buffer, buffer_src, buffer_size );

  return MONITOR_OK;
}
