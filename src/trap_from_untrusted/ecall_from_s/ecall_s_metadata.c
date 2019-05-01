#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

__attribute__((section(".text.sm.untrusted_trap")))

api_result_t ecall_create_metadata_region(dram_region_id_t id) {
   // Check argument validity
   if(id < NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure	
   dram_region_t *r_ptr = &(sm_globals.regions[id]);

   if(!aquireLock(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be free
   if(r_ptr->state != dram_region_free) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   // Set the DRAM region type and state
   r_ptr->type  = metadata_region; 
   r_ptr->state = dram_region_owned;

   // Initialize the metadata page map
   metadata_page_map_t page_map = (metadata_page_map_t) id;

   for(int i = 0; i < NUM_METADATA_PAGES_PER_REGION; i++) {
      page_map[i] = 0;
   }

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}

uint64_t ecall_metadata_region_pages() {
   return NUM_METADATA_PAGES_PER_REGION;
}

uint64_t ecall_metadata_region_start() {
   uint64_t size_mpm = sizeof(metadata_page_map_entry_t) * ecall_metadata_region_pages();
   return (size_mpm / SIZE_PAGE) + ((size_mpm % SIZE_PAGE) ? 1 : 0); // Rouded up division. No +1 as indexing starts at 0
}

uint64_t ecall_thread_metadata_pages() {
   uint64_t size_th = sizeof(thread_t);
   return (size_th / SIZE_PAGE) + ((size_th % SIZE_PAGE) ? 1 : 0); // Rouded up division
}

uint64_t ecall_enclave_metadata_pages(uint64_t mailbox_count) {
   uint64_t size_e = sizeof(enclave_t) + (sizeof(mailbox_t) * mailbox_count);
   return (size_e / SIZE_PAGE) + ((size_e % SIZE_PAGE) ? 1 : 0); // Rouded up division
}
