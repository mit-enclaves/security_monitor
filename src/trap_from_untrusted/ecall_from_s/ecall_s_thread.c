#include <ecall_s.h>
#include <clib/clib.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t ecall_allocate_thread(enclave_id_t enclave_id, thread_id_t thread_id) {

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

   uint64_t num_metadata_pages = ecall_thread_metadata_pages();

   if((METADATA_IDX(thread_id) + num_metadata_pages) >= ecall_metadata_region_pages()) {
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      if((page_map[i] & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) { 
         return monitor_invalid_state;
      }
   }

   // Initiate the metadata page map

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
      page_map[i] |= (enclave_id << ENTRY_OWNER_ID_OFFSET) 
         | (metadata_thread & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1));
   }

   return monitor_ok;
}

struct inputs_load_thread_t{
   uintptr_t entry_pc;
   uintptr_t entry_stack;
   uintptr_t fault_pc;
   uintptr_t fault_stack;
};

api_result_t ecall_load_thread(enclave_id_t enclave_id, thread_id_t thread_id,
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
  
   api_result_t ret = ecall_allocate_thread(enclave_id, thread_id);

   if(ret != monitor_ok) {
      releaseLock(dram_region_ptr->lock);
      return ret;
   }

   thread_t *thread = (thread_t *) thread_id;

   thread->is_scheduled.flag   = 0;
   thread->aex_present    = false;
   thread->untrusted_pc   = 0;
   thread->untrusted_sp   = 0;
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

   // Update measurement
   struct inputs_load_thread_t inputs = {0};
   inputs.entry_pc = entry_pc;
   inputs.entry_stack = entry_stack;
   inputs.fault_pc = fault_pc;
   inputs.fault_stack = fault_stack;

   sha3_update(&(enclave->sha3_ctx), &(inputs), sizeof(struct inputs_load_thread_t));

   releaseLock(er_ptr->lock);

   return monitor_ok;
}

api_result_t ecall_assign_thread(enclave_id_t enclave_id, thread_id_t thread_id) {
   
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
  
   api_result_t ret = ecall_allocate_thread(enclave_id, thread_id);

   releaseLock(dram_region_ptr->lock);
   return ret;
}

api_result_t ecall_delete_thread(thread_id_t thread_id) {
   // TODO: check that thread_id is a valid thread_id (owner?)

   dram_region_t * tr_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(tr_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
   
   metadata_page_map_t page_map = (metadata_page_map_t) tr_ptr;
   
   enclave_id_t owner_id = (page_map[METADATA_IDX(thread_id)]) >> ENTRY_OWNER_ID_OFFSET;
   
   // Check thread_id validity   
   api_result_t ret = is_valid_thread(owner_id, thread_id);

   if(ret != monitor_ok) {
      releaseLock(tr_ptr->lock); // Release Lock
      return ret;
   }

   // Check that thread is not scheduled
   thread_t *thread = (thread_t *) thread_id;

   if(aquireLock(thread->is_scheduled)) {
      releaseLock(tr_ptr->lock);
      return monitor_invalid_state;
   }

   uint64_t num_metadata_pages = ecall_thread_metadata_pages();
   
   // Clean the metadata page map
   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
   }

   // Clean the metadata it-self
   memset((void *) thread_id, 0, sizeof(thread_t));

   releaseLock(tr_ptr->lock);

   // Decrease the owning enclave's thread_count
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(owner_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   ((enclave_t *) owner_id)->thread_count--;

   releaseLock(er_ptr->lock);
   
   return monitor_ok;
}
