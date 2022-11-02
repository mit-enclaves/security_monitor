#include <sm.h>

api_result_t sm_internal_enclave_load_page (enclave_id_t enclave_id,
  phys_ptr_t phys_addr,
  uintptr_t virtual_addr,
  uintptr_t os_addr,
  uintptr_t acl) {

  // Validate inputs
  // ---------------

  /*
    - enclave_id must be valid
    - enclave must be in state ENCLAVE_STATE_PAGE_TABLES_LOADED or ENCLAVE_STATE_PAGE_DATA_LOADED
    - phys_addr must be page alligned
    - phys_addr must be greater than the last physical address loaded
    - phys_addr must point to a region owned by the enclave
    - virtual addr must be within the enclave evrange
    - os_addr must be page aligned
    - os_addr must point to a region own by the OS
    - acl must be valid and a leaf acl
    -
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

  // enclave must be in state ENCLAVE_STATE_PAGE_TABLES_LOADED or ENCLAVE_STATE_PAGE_DATA_LOADED
  if((enclave_metadata->init_state != ENCLAVE_STATE_PAGE_TABLES_LOADED)
    && (enclave_metadata->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // phys_addr must be page alligned
  if(phys_addr % PAGE_SIZE) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // phys_addr must be greater than the last physical address loaded
  if(enclave_metadata->last_phys_addr_loaded > phys_addr) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that phys_addr points into a DRAM region owned by the enclave
  if(region_owner(addr_to_region_id(phys_addr)) != enclave_id){
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // Check virtual addr validity
  if(((virtual_addr & enclave_metadata->platform_csr.ev_mask) != enclave_metadata->platform_csr.ev_base) ||
       (((virtual_addr + PAGE_SIZE) & enclave_metadata->platform_csr.ev_mask) != enclave_metadata->platform_csr.ev_base)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // os_addr must be page alligned
  if(os_addr % PAGE_SIZE) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // Check that os_addr points into a DRAM region owned by the os
  if(region_owner(addr_to_region_id(os_addr)) != OWNER_UNTRUSTED){
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // ACL must be valid
  if(((acl & PTE_V) == 0) ||
       (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W)) ||
       (((acl & PTE_R) == 0) && ((acl & PTE_X) == 0))         ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr region and the os_addr region

  if(!add_lock_region(addr_to_region_id(phys_addr), &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  if(!add_lock_region(addr_to_region_id(os_addr), &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Load page table entry in page table and check arguments
  result = load_page_table_entry(enclave_id, phys_addr, virtual_addr, 0, acl, &locked_regions); // TODO: Are loaded pages always kilo pages?
  if ( MONITOR_OK != result ) {
    unlock_regions(&locked_regions);
    return result;
  }

  // Update the last physical address loaded
  enclave_metadata->last_phys_addr_loaded = phys_addr + PAGE_SIZE;

  // Update the enclave state
  enclave_metadata->init_state = ENCLAVE_STATE_PAGE_DATA_LOADED;

  // Load page
  memcpy((void *) phys_addr, (void *) os_addr, PAGE_SIZE);

  // Update measurement
  hash_extend(&enclave_metadata->hash_context, &virtual_addr, sizeof(virtual_addr));
  hash_extend(&enclave_metadata->hash_context, &acl, sizeof(acl));
  hash_extend(&enclave_metadata->hash_context, (const void *) os_addr, PAGE_SIZE);

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
