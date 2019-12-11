static api_result_t enclave_load_page_table_entry (enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   // Check that phys_addr is page alligned
   if(phys_addr % SIZE_PAGE) {
      return monitor_invalid_value;
   }

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *)  enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(enclave->last_phys_addr_loaded > phys_addr) {
      return monitor_invalid_value;
   }

   // Check virtual addr validity
   if(((virtual_addr & enclave->evmask) != enclave->evbase) ||
         (((virtual_addr + SIZE_PAGE) & enclave->evmask) != enclave->evbase)) {
      return monitor_invalid_value;
   }

   // Check that phys_addr points into a DRAM region owned by the enclave
   if(!owned(phys_addr, enclave_id)){
      return monitor_invalid_state;
   }

   // Initialize page table entry
   if(level == 3) {
      enclave->eptbr = phys_addr >> SHIFT_PAGE;
      enclave->eptbr |= 8lu << SATP_MODE_SHIFT;
   }
   else {
      uintptr_t pte_base = (enclave->eptbr & SATP_PPN_MASK) << SHIFT_PAGE;
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

   enclave->last_phys_addr_loaded = phys_addr;

   return monitor_ok;
}
