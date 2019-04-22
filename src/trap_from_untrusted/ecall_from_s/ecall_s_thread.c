#include <clib/clib.h>
#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t allocate_thread(enclave_id_t enclave_id, thread_id_t thread_id) {

   // Check that thread_id is page alligned
   if(thread_id % SIZE_PAGE) {
      return monitor_invalid_value;
   }

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);
   metadata_page_map_t page_map = (metadata_page_map_t) dram_region_ptr;
   
   // Check that dram region is an metadata region
   if(dram_region_ptr->type != metadata_region) {
      return monitor_invalid_value;
   }

   // Check metadata pages availability

   uint64_t num_metadata_pages = thread_metadata_pages();

   if((METADATA_IDX(thread_id) + num_metadata_pages) >= metadata_region_pages()) {
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      if((page_map[i] & ((1u << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) { 
         return monitor_invalid_state;
      }
   }

   // Initiate the metadata page map

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
      page_map[i] |= (enclave_id << ENTRY_OWNER_ID_OFFSET) 
         | (metadata_thread & ((1u << ENTRY_OWNER_ID_OFFSET) - 1));
   }

   return monitor_ok;
}

api_result_t load_thread(enclave_id_t enclave_id, thread_id_t thread_id,
    uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc,
    uintptr_t fault_stack) {

   // TODO check other arguments validity

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;
   
   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
  
   api_result_t ret = allocate_thread(enclave_id, thread_id);

   if(ret != monitor_ok) {
      releaseLock(dram_region_ptr->lock);
      return ret;
   }

   thread_t *thread = (thread_t *) thread_id;

   thread->is_scheduled   = false;
   thread->aes_present    = 0; // TODO: What is this?
   thread->untrusted_pc   = 0;
   thread->untrusted_sp   = 0;
   thread->page_table_ptr = 0;
   thread->entry_pc       = entry_pc;
   thread->entry_sp       = entry_stack;
   thread->fault_pc       = fault_pc;
   thread->fault_sp       = fault_stack;

   for(int i = 0; i < NUM_REGISTERS; i++) {
      thread->fault_state[i] = 0;
      thread->aex_state[i]   = 0;
   }

   releaseLock(dram_region_ptr->lock); // Release Lock

   // Increase the enclave's thread_count
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   enclave->thread_count++;

   // TODO: Update measurement?

   releaseLock(er_ptr->lock);

   return monitor_ok;
}

api_result_t assign_thread(enclave_id_t enclave_id, thread_id_t thread_id) {
   
   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;
   
   // Check that the enclave is initialized.
   if(!enclave->initialized) {
      return monitor_invalid_state;
   }

   // TODO: Check that enclave hasn't been killed

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
  
   api_result_t ret = allocate_thread(enclave_id, thread_id);

   releaseLock(dram_region_ptr->lock);
   return ret;
}

api_result_t delete_thread(thread_id_t thread_id) {
   // TODO: check that thread_id is a valid thread_id (owner?)

   if(thread_id % SIZE_PAGE) {
      return false;
   }

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an metadata region
   if(dram_region_ptr->type != metadata_region) { 
      releaseLock(dram_region_ptr->lock); // Release Lock
      return false;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) dram_region_ptr;

   uint64_t num_metadata_pages = thread_metadata_pages();

   if((METADATA_IDX(thread_id) + num_metadata_pages) >= metadata_region_pages()) {
      releaseLock(dram_region_ptr->lock);
      return monitor_invalid_value;
   }

   enclave_id_t owner_id = (page_map[METADATA_IDX(thread_id)]) >> ENTRY_OWNER_ID_OFFSET;

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      if((page_map[i] >> ENTRY_OWNER_ID_OFFSET) != owner_id) { 
         releaseLock(dram_region_ptr->lock);
         return monitor_invalid_state;
      }
      if((page_map[i] & ((1u << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_thread) { 
         releaseLock(dram_region_ptr->lock);
         return monitor_invalid_state;
      }
   }
   
   // Check that thread is not scheduled
   thread_t *thread = (thread_t *) thread_id;

   if(aquireLock(thread->is_scheduled)) {
      releaseLock(dram_region_ptr->lock);
      return monitor_invalid_state;
   }

   // Clean the metadata page map
   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
   }

   // Clean the metadata it-self
   memset((void *) thread_id, 0, sizeof(thread_t));

   releaseLock(dram_region_ptr->lock);

   // Decrease the owning enclave's thread_count
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(owner_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   ((enclave_t *) owner_id)->thread_count--;

   releaseLock(er_ptr->lock);
   
   return monitor_ok;
}


