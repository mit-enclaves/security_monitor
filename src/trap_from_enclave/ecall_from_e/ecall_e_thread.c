#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <clib/clib.h> 
#include <sm_util/sm_util.h>

api_result_t accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr) {

   // Get the caller id
   enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;
   
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

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      if((page_map[i] >> ENTRY_OWNER_ID_OFFSET) != caller_id) { 
         releaseLock(dram_region_ptr->lock);
         return monitor_invalid_state;
      }
      if((page_map[i] & ((1u << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_thread) { 
         releaseLock(dram_region_ptr->lock);
         return monitor_invalid_state;
      }
   }

   // TODO: How to check that this thread is not already initialized?

   // TODO: Check thread_info_t validity?

   // Copy the metadata
   memcpy((void *) thread_id, (void *) thread_info_addr, sizeof(thread_t));

   releaseLock(dram_region_ptr->lock);
   
   // Increase the enclave's thread_count
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(caller_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   ((enclave_t *) caller_id)->thread_count++;

   releaseLock(er_ptr->lock);

   return monitor_ok;
}
