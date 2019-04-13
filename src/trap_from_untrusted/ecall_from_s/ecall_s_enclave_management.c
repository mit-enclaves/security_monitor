#include <clib.h>
#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t create_enclave(enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug) {
   // TODO: Check all arguments validity

   // Check that enclave_id is page alligned	
   if(enclave_id % SIZE_PAGE) {
      return monitor_invalid_value;
   }

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) enclave_id)]);	

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an metadata region
   if(dram_region_ptr->type != metadata_region) {
      releaseLock(dram_region_ptr->lock); // Release Lock
      return monitor_invalid_value;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) dram_region_ptr;

   // Check metadata pages availability

   uint64_t num_metadata_pages = enclave_metadata_pages(mailbox_count);

   if((METADATA_IDX(enclave_id) + num_metadata_pages) >= metadata_region_pages) {
      releaseLock(dram_region_ptr->lock); // Release Lock
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      if((page_map[i] & ((1u << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) { 
         releaseLock(dram_region_ptr->lock); // Release Lock
         return monitor_invalid_state;
      }
   }

   // Initiate the metadata page map

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
      page_map[i] |= (enclave_id << ENTRY_OWNER_ID_OFFSET) 
         | (metadata_enclave & ((1u << ENTRY_OWNER_ID_OFFSET) - 1))
   }

   enclave_t *enclave = (enclave_t *) enclave_id;

   enclave->initialized = 0;
   enclave->debug = debug;
   enclave->thread_cout = 0;
   enclave->dram_bitmap = 0;
   enclave->last_phys_addr_loaded = 0;
   enclave->evbase = evbase;
   enclave->evmask = evmask;

   // Initialize mailboxes
   enclave->mailbox_count = mailbox_count;
   enclave->mailbox_array = (mailbox_t *) (enclave_id + sizeof(enclave_t));
   for(int i = 0; i < enclave->mailbox_count; i++) {
      (enclave->mailbox_array[i]).sender = 0;
      (enclave->mailbox_array[i]).has_message = false;
      for(int j = 0; j < MAILBOX_SIZE; j++) {
         (enclave->mailbox_array[i]).message[j] = 0;
      }
   }

   // TODO: Update measurement
   enclave->measurement = 0;

   releaseLock(dram_region_ptr->lock); // Release Lock

   return monitor_ok;
}

api_result_t load_page_table_entry(enclave_id_t enclave_id, uintptr_t phys_addr, 
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   // Check that phys_addr is page alligned
   if(phys_addr % PAGE_SIZE) {
      return monitor_invalid_value;
   }

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   (enclave_t *) enclave = enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(enclave->last_phys_addr_loaded <= phys_addr) {
      return monitor_invalid_value;
   }

   // Check virtual addr validity
   if(((virtual_addr & enclave->evmask) != enclave->evbase) ||
         (((virtual_addr + PAGE_SIZE) & enclave->evmask) != enclave->evbase)) {
      return monito_invalid_value;
   }

   // Check that phys_addr points into a DRAM region owned by the enclave
   if(!owned(phys_addr, enclave_id)){
      return monitor_invalid_state;
   }

   // Initialize page table entry
   if(level == 3) {
      enclave->eptbr = phys_addr;
   }
   else {
      uintptr_t pte_base = enclave->eptbr;
      if(!owned(pte_base, enclave_id)){
         return monitor_invalid_state;
      }

      for(i = 2; i >= level; i--) {
         uint64_t * pte_addr = pte_base +
            ((v_addr >> (PAGE_OFFSET + (PN_OFFSET * i))) & PN_MASK);

         if(i != level) {
            uint64_t pte_acl = (*pte_addr) & ACL_MASK;
            if((pte_acl & PTE_V == 0) ||
                  ((pte_acl & PTE_R == 0) && (pte_acl & PTE_W == PTE_W)) || 
                  (pte_acl & PTE_R == PTE_R) || (pte_acl & PTE_X == PTE_X)    ) {
               return monitor_invalid_state;
            }

            pte_base = (((*pte_addr) >> PAGE_ENTRY_ACL_OFFSET) & PPNs_MASK) << PAGE_OFFSET;

            if(!owned(pte_base, enclave_id)){
               return monitor_invalid_state;
            }
         }
         else {
            *pte_addr = 0 |
               ((phys_addr >> PAGE_OFFSET) & (PPNs_MASK) << PAGE_ENTRY_ACL_OFFSET) |
               (acl & ACL_MASK);
         }
      }
   }

   enclave->last_phys_addr_loaded = phys_addr;

   return monitor_ok;
}

api_result_t load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr, 
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   if(level > 3) {
      return monitor_invalid_value;
   }

   // Check that ACL is valid and is a leaf ACL
   if((acl & PTE_V == 0) ||
         ((acl & PTE_R == 0) && (acl & PTE_W == PTE_W)) || 
         (acl & PTE_R == PTE_R) || (acl & PTE_X == PTE_X)    ) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_global.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monito_concurent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, level, acl);

   if(ret != monitor_ok) {
      releaseLock(er_ptr->lock);
      return ret;
   }

   // TODO: Update measurement

   releaseLock(er_ptr->lock);

   return monitor_ok
}

api_result_t load_page(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl) {

   // Check that ACL is valid anf is not a leaf ACL
   if((acl & PTE_V == 0) ||
         ((acl & PTE_R == 0) && (acl & PTE_W == PTE_W)) || 
         ((acl & PTE_R == 0) && (acl & PTE_X == 0))         ) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_global.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monito_concurent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, 0, acl); // TODO: Are loaded pages always kilo pages?

   if(ret != monitor_ok) {
      releaseLock(er_ptr->lock);
      return ret;
   }

   // Load page
   // TODO: Check os_addr
   memcpy(phys_addr, os_addr, SIZE_PAGE);

   // TODO: Update measurement

   releaseLock(er_ptr->lock);

   return monitor_ok;
}
