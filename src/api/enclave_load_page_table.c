#include <sm.h>

api_result_t sm_enclave_load_page_table (enclave_id_t enclave_id,
  phys_ptr_t phys_addr,
  uintptr_t virtual_addr,
  uint64_t level,
  uintptr_t acl) {

  // Validate inputs
  // ---------------

  /*
    - enclave_id must be valid
    - enclave must be in state ENCLAVE_STATE_HANDLER_LOADED or ENCLAVE_STATE_PAGE_TABLES_LOADED
    - phys_addr must be page alligned
    - phys_addr must be greater than the last physical address loaded
    - phys_addr must point to a region owned by the enclave
    - virtual addr must be within the enclave evrange
    - level must be no greater than 3
    - acl must be valid
  */

  // Lock the enclave's metadata's region and the phys_addr region

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  // enclave_id must be valid
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // enclave must be in state ENCLAVE_STATE_HANDLER_LOADED or ENCLAVE_STATE_PAGE_TABLES_LOADED
  if((enclave_metadata->init_state != ENCLAVE_STATE_HANDLER_LOADED)
    && (enclave_metadata->init_state != ENCLAVE_STATE_PAGE_TABLES_LOADED)) {
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

  // phys_addr must point to a region owned by the enclave
  if(sm_region_owner(addr_to_region_id(phys_addr)) != enclave_id){
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // Check virtual addr validity
  if(((virtual_addr & enclave_metadata->ev_mask) != enclave_metadata->ev_base) ||
       (((virtual_addr + PAGE_SIZE) & enclave_metadata->ev_mask) != enclave_metadata->ev_base)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // level must be no greater than 3
  if(level > 3) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // ACL must be valid
  if(((acl & PTE_V) == 0) ||
       (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W))) {
    return MONITOR_INVALID_VALUE;
  }

  // Lock the phys_addr region
  if(!add_lock_region(addr_to_region_id(phys_addr), &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_VALUE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Load page table entry in page table and check arguments
  result = load_page_table_entry(enclave_id, phys_addr, virtual_addr, level, acl);
  if ( MONITOR_OK != result ) {
    unlock_regions(&locked_regions);
    return result;
  }

  // Update the last physical address loaded
  enclave_metadata->last_phys_addr_loaded = phys_addr;

  // Update measurement
  hash_extend(&enclave_metadata->hash_context, &virtual_addr, sizeof(virtual_addr));
  hash_extend(&enclave_metadata->hash_context, &level, sizeof(level));
  hash_extend(&enclave_metadata->hash_context, &acl, sizeof(acl));

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
