#include <sm.h>

api_result_t load_page_table_entry (enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl, region_map_t *locked_regions) {

  // Validate inputs
  // ---------------

  // NOTE: Inputs are already deemed valid by the caller

  // Apply state transition
  // ----------------------

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // Initialize page table entry
  if(level == 3) {
    enclave_metadata->platform_csr.eptbr = phys_addr >> PAGE_SHIFT;
    enclave_metadata->platform_csr.eptbr |= SATP_MODE_SV39 << SATP_MODE;
  }
  else {
    uintptr_t pte_base = (enclave_metadata->platform_csr.eptbr & SATP_PPN_MASK) << PAGE_SHIFT;
    int pte_region_id = addr_to_region_id(pte_base);
    if( !(((locked_regions->flags[pte_region_id]) && (region_owner_lock_free(pte_region_id) == enclave_id)) 
          ||
          ((!locked_regions->flags[pte_region_id]) && (region_owner(pte_region_id) == enclave_id)))
      ){
      return MONITOR_INVALID_STATE;
    }

    for(int i = 2; i >= (int) level; i--) {
      uint64_t * pte_addr = (uint64_t *) ((uint64_t) pte_base +
          (((virtual_addr >> (PAGE_SHIFT + (PN_OFFSET * i))) & PN_MASK) * PTE_SIZE));

      if(i != level) {
        uint64_t pte_acl = (*pte_addr) & ACL_MASK;
        if(((pte_acl & PTE_V) == 0) ||
            (((pte_acl & PTE_R) == 0) && ((pte_acl & PTE_W) == PTE_W)) ||
            ((pte_acl & PTE_R) == PTE_R) || ((pte_acl & PTE_X) == PTE_X)    ) {
          return MONITOR_INVALID_STATE;
        }

        pte_base = (((*pte_addr) & PPNs_MASK) >> PAGE_ENTRY_ACL_OFFSET)  << PAGE_SHIFT;
        pte_region_id = addr_to_region_id(pte_base);
        if( !(((locked_regions->flags[pte_region_id]) && (region_owner_lock_free(pte_region_id) == enclave_id)) 
              ||
              ((!locked_regions->flags[pte_region_id]) && (region_owner(pte_region_id) == enclave_id)))
          ){
          return MONITOR_INVALID_STATE;
        }
      }
      else {
        *pte_addr = 0 |
          ((((phys_addr >> PAGE_SHIFT) << PAGE_ENTRY_ACL_OFFSET) & (PPNs_MASK)) |
           (acl & ACL_MASK));
      }
    }
  }

  return MONITOR_OK;
}
