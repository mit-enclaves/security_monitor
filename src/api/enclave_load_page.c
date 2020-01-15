#include <sm.h>

api_result_t sm_enclave_load_page (enclave_id_t enclave_id,
  phys_ptr_t phys_addr,
  uintptr_t virtual_addr,
  uintptr_t os_addr,
  uintptr_t acl) {

  // Validate inputs
  // ---------------

  /*
    - enclave_id must be valid
    - enclave must be in state
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

  // <TRANSACTION>
  // enclave_id must be valid
  api_result_t result = lock_region_iff_valid_enclave( enclave_id );
  if ( MONITOR_OK != result ) {
    return result;
  }

  uint64_t region_id = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  if((enclave_metadata->init_state != ENCLAVE_STATE_PAGE_TABLES_LOADED)
    && (enclave_metadata->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED)) {
    unlock_region(region_id);
    return MONITOR_INVALID_STATE;
  }

  // phys_addr must be page alligned
  if(phys_addr % PAGE_SIZE) {
    unlock_region(region_id);
    return MONITOR_INVALID_VALUE;
  }

  // phys_addr must be greater than the last physical address loaded
  if(enclave_metadata->last_phys_addr_loaded > phys_addr) {
    unlock_region(region_id);
    return MONITOR_INVALID_VALUE;
  }

  // Check that phys_addr points into a DRAM region owned by the enclave
  if(sm_region_owner(addr_to_region_id(phys_addr)) != enclave_id){
    unlock_region(region_id);
    return MONITOR_INVALID_STATE;
  }

  // Check virtual addr validity
  if(((virtual_addr & enclave_metadata->ev_mask) != enclave_metadata->ev_base) ||
       (((virtual_addr + PAGE_SIZE) & enclave_metadata->ev_mask) != enclave_metadata->ev_base)) {
    unlock_region(region_id);
    return MONITOR_INVALID_VALUE;
  }

  // os_addr must be page alligned
  if(os_addr % PAGE_SIZE) {
    unlock_region(region_id);
    return MONITOR_INVALID_VALUE;
  }

  // Check that phys_addr points into a DRAM region owned by the enclave
  if(sm_region_owner(addr_to_region_id(os_addr)) != OWNER_UNTRUSTED){
    unlock_region(region_id);
    return MONITOR_INVALID_STATE;
  }

  // ACL must be valid
  if(((acl & PTE_V) == 0) ||
       (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W)) ||
       (((acl & PTE_R) == 0) && ((acl & PTE_X) == 0))         ) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr region and the os_addr region

  if(!lock_region(addr_to_region_id(phys_addr))) {
    unlock_region(region_id);
    return MONITOR_INVALID_VALUE;
  }

  if(!lock_region(addr_to_region_id(os_addr))) {
    unlock_region(region_id);
    unlock_region(addr_to_region_id(phys_addr));
    return MONITOR_INVALID_VALUE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Load page table entry in page table and check arguments
  result = load_page_table_entry(enclave_id, phys_addr, virtual_addr, 0, acl); // TODO: Are loaded pages always kilo pages?
  if ( MONITOR_OK != result ) {
    unlock_region(region_id);
    unlock_region(addr_to_region_id(phys_addr));
    unlock_region(addr_to_region_id(os_addr));
    return result;
  }

  // Update the last physical address loaded
  enclave_metadata->last_phys_addr_loaded = phys_addr;

  // Load page
  memcpy((void *) phys_addr, (void *) os_addr, PAGE_SIZE);

  // Update measurement
  hash_extend(&enclave_metadata->hash_context, &virtual_addr, sizeof(virtual_addr));
  hash_extend(&enclave_metadata->hash_context, &acl, sizeof(acl));
  hash_extend(&enclave_metadata->hash_context, (const void *) os_addr, PAGE_SIZE);

  // Release locks
  unlock_region(region_id);
  unlock_region(addr_to_region_id(phys_addr));
  unlock_region(addr_to_region_id(os_addr));
  // </TRANSACTION>

  return MONITOR_OK;
}
