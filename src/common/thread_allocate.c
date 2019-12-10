#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t thread_allocate (enclave_id_t enclave_id, thread_id_t thread_id) {

   // Check that thread_id is page alligned
   if(thread_id % PAGE_SIZE) {
      return monitor_invalid_value;
   }

   dram_region_t * dram_region_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);
   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(thread_id);

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
