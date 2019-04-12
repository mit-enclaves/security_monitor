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


api_result_t load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr, 
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   // Check that phys_addr is page alligned
   if(phys_addr % PAGE_SIZE) {
      return monitor_invalid_value;
   }

   if(level > 3) {
      return monitor_invalid_value;
   }

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_global.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monito_concurent_call;
   }

   // Check that the enclave is not initialized.
   if(((enclave_t *) enclave_id)->initialized) {
      releaseLock(er_ptr->lock);
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(((enclave_t *) enclave_id)->last_phys_addr_loaded <= phys_addr) {
      releaseLock(er_ptr->lock);
      return monitor_invalid_value;
   }
   
   // Check that phys_addr points into a DRAM region owned by the enclave
   dram_region_t *r_ptr = &(sm_global.regions[REGION_IDX(phys_addr)]);

   if(!aquireLock(r_ptr->lock)) {
      releaseLock(er_ptr->lock);
      return monito_concurent_call;
   }

   if((r_ptr->type != enclave_region) || (r_ptr->state != dram_region_owned) || (r_ptr->owner != enclave_id)) {
      releaseLock(er_ptr->lock);
      releaseLock(r_ptr->lock);
      return monitor_invalid_state;
   }

   // Check virtual addr validity
   // Check acl validity
   // Copy the page table
   // Update the measurement
      
   releaseLock(er_ptr->lock);
   releaseLock(r_ptr->lock);
   
   return monitor_ok;
}
