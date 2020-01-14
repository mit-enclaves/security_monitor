#include <sm.h>

#define PTE_V (1ul)
#define PTE_R (1ul << 1)
#define PTE_W (1ul << 2)
#define PTE_X (1ul << 3)

#define SATP_MODE_SV39 (8ul)
#define SATP_MODE (60)

#define PTE_SIZE (8)
#define PN_OFFSET (9)
#define PPN2_OFFSET (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

#define PN_MASK ((1ul << PN_OFFSET) - 1)
#define PPN2_MASK ((1ul << PPN2_OFFSET) - 1)
#define ACL_MASK ((1ul << PAGE_ENTRY_ACL_OFFSET) - 1)
#define SATP_PPN_MASK ((PPN2_MASK << (PN_OFFSET * 2)) | (PN_MASK << PN_OFFSET) | PN_MASK)
#define PPNs_MASK (SATP_PPN_MASK << PAGE_ENTRY_ACL_OFFSET)


#DEFINE SATP_PPN_MASK

static api_result_t enclave_load_page_table_entry (enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

  // Validate inputs
  // ---------------

  /*
    - enclave_id must be valid
    - phys_addr must be page alligned
    - phys_addr must be greater than the last physical address loaded
    - phys_addr must point to a region owned by the enclave
    - virtual addr must be within the enclave evrange
  */

  // Lock the physical address region

  // enclave_id must be valid
  if(!is_valid_enclave(enclave_id)) {
    return MONITOR_INVALID_VALUE;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // phys_addr must be page alligned
  if(phys_addr % PAGE_SIZE) {
    return MONITOR_INVALID_VALUE;
  }

  // phys_addr must be greater than the last physical address loaded
  if(enclave_metadata->last_phys_addr_loaded > phys_addr) {
    return MONITOR_INVALID_VALUE;
  }

  // Check that phys_addr points into a DRAM region owned by the enclave
  if(sm_region_owner(addr_to_region_id(phys_addr)) != enclave_id){
    return MONITOR_INVALID_STATE;
  }

  // Check virtual addr validity
  if(((virtual_addr & enclave_metadata->ev_mask) != enclave_metadata->ev_base) ||
       (((virtual_addr + SIZE_PAGE) & enclave->ev_mask) != enclave->ev_base)) {
    return MONITOR_INVALID_VALUE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Initialize page table entry
  if(level == 3) {
    enclave_metadata->eptbr = phys_addr >> PAGE_SHIFT;
    enclave_metadata->eptbr |= SATP_MODE_SV39 << SATP_MODE;
  }
  else {
    uintptr_t pte_base = (enclave_metadata->eptbr & SATP_PPN_MASK) << SHIFT_PAGE;
    if(!owned(pte_base, enclave_id)){
       return monitor_invalid_state;
    }

    for(int i = 2; i >= (int) level; i--) {
       uint64_t * pte_addr = (uint64_t *) ((uint64_t) pte_base +
          (((virtual_addr >> (PAGE_OFFSET + (PN_OFFSET * i))) & PN_MASK) * PTE_SIZE));

       if(i != level) {
          uint64_t pte_acl = (*pte_addr) & ACL_MASK;
          if(((pte_acl & PTE_V) == 0) ||
                (((pte_acl & PTE_R) == 0) && ((pte_acl & PTE_W) == PTE_W)) ||
                ((pte_acl & PTE_R) == PTE_R) || ((pte_acl & PTE_X) == PTE_X)    ) {
             return monitor_invalid_state;
          }

          pte_base = (((*pte_addr) & PPNs_MASK) >> PAGE_ENTRY_ACL_OFFSET)  << PAGE_OFFSET;

          if(!owned(pte_base, enclave_id)){
             return monitor_invalid_state;
          }
       }
       else {
          *pte_addr = 0 |
             ((((phys_addr >> PAGE_OFFSET) << PAGE_ENTRY_ACL_OFFSET) & (PPNs_MASK)) |
             (acl & ACL_MASK));
       }
    }
  }

  enclave_metadata->last_phys_addr_loaded = phys_addr;

  return monitor_ok;
}
