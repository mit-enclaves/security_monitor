#include <sm.h>

TODO

api_result_t sm_thread_delete (thread_id_t thread_id) {
   // TODO: check that thread_id is a valid thread_id (owner?)

   dram_region_t * tr_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(tr_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(thread_id);

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
   dram_region_t *er_ptr = &(SM_GLOBALS.regions[REGION_IDX(owner_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   ((enclave_metadata_t *) owner_id)->thread_count--;

   releaseLock(er_ptr->lock);

   return monitor_ok;
}
