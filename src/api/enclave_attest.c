#include <sm.h>

api_result_t sm_internal_enclave_attest (enclave_id_t enclave_id,
    phys_ptr_t phys_addr_m,
    phys_ptr_t phys_addr_s) {

  // Validate inputs
  // ---------------

  /*
     - enclave_id must be valid
     - enclave must be in state ENCLAVE_STATE_INITIALIZED
     - the output buffers must fit entirely in one region
     - the output buffers region must belong to the caller
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

  ////// Check phys_addr_m and phys_addr_s
  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

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

  public_key_t * pk = &(sm->keys.software_public_key);
  secret_key_t * sk = &(sm->keys.software_secret_key);

  signature_t attestation;
  sign(&(enclave_metadata->measurement), size_m, pk, sk, &attestation);

  // Copy outputs
  memcpy((void *) phys_addr_m, (void *) &(enclave_metadata->measurement), size_m);
  memcpy((void *) phys_addr_s, (void *) &(attestation), size_s);

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  //TODO: need to flush the L2

  return MONITOR_OK;
}
