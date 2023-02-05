#include <sm.h>

api_result_t sm_internal_enclave_get_keys (
    phys_ptr_t phys_addr_m,
    phys_ptr_t phys_addr_pk,
    phys_ptr_t phys_addr_sk,
    phys_ptr_t phys_addr_s) {

  // Validate inputs
  // ---------------

  /*
     - the caller must be a valid encalve
     - the output buffers must fit entirely in one region
     - the output buffers region must belong to the caller
     */

  // Lock the enclave's metadata's region, the phys_addr region and the os_addr region
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

  ////// Check phys_addr_m, phys_addr_pk, phys_addr_sk and phys_addr_s
  
  //// phys_addr_m
  size_t size_m = sizeof(hash_t);

  // Check that the buffers fit entirely in one region
  uint64_t region_id_m = addr_to_region_id(phys_addr_m);
  if ( !is_valid_region_id(region_id_m) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id_m != addr_to_region_id(phys_addr_m+size_m-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr_m regions
  sm_region_t * region_m_metadata = &sm->regions[region_id_m];
  if(!add_lock_region(region_id_m, &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that the phys_addr_* point into DRAM regions owned by the caller
  if ( region_m_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }
  if ( region_m_metadata->owner != caller ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }
  
  //// phys_addr_pk
  size_t size_pk = sizeof(public_key_t);

  // Check that the buffers fit entirely in one region
  uint64_t region_id_pk = addr_to_region_id(phys_addr_pk);
  if ( !is_valid_region_id(region_id_pk) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id_pk != addr_to_region_id(phys_addr_pk+size_pk-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr_pk regions
  sm_region_t * region_pk_metadata = &sm->regions[region_id_pk];
  if(!add_lock_region(region_id_pk, &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that the phys_addr_* point into DRAM regions owned by the caller
  if ( region_pk_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }
  if ( region_pk_metadata->owner != caller ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  //// phys_addr_sk
  size_t size_sk = sizeof(secret_key_t);

  // Check that the buffers fit entirely in one region
  uint64_t region_id_sk = addr_to_region_id(phys_addr_sk);
  if ( !is_valid_region_id(region_id_sk) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id_sk != addr_to_region_id(phys_addr_sk+size_sk-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr_sk regions
  sm_region_t * region_sk_metadata = &sm->regions[region_id_sk];
  if(!add_lock_region(region_id_sk, &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that the phys_addr_* point into DRAM regions owned by the caller
  if ( region_sk_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }
  if ( region_sk_metadata->owner != caller ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  //// phys_addr_s
  size_t size_s = sizeof(signature_t);

  // Check that the buffers fit entirely in one region
  uint64_t region_id_s = addr_to_region_id(phys_addr_s);
  if ( !is_valid_region_id(region_id_s) ) {
    return MONITOR_INVALID_VALUE;
  }

  if ( region_id_s != addr_to_region_id(phys_addr_s+size_s-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr_s regions
  sm_region_t * region_s_metadata = &sm->regions[region_id_s];
  if(!add_lock_region(region_id_s, &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that the phys_addr_s point into a DRAM region owned by the caller
  if ( region_s_metadata->state != REGION_STATE_OWNED ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }
  if ( region_s_metadata->owner != caller ) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy outputs
  memcpy((void *) phys_addr_m, (void *) &(enclave_metadata->measurement), size_m);
  memcpy((void *) phys_addr_pk, (void *) &(enclave_metadata->public_key), size_pk);
  memcpy((void *) phys_addr_sk, (void *) &(enclave_metadata->secret_key), size_pk);
  memcpy((void *) phys_addr_s, (void *) &(enclave_metadata->attestation), size_s);

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
