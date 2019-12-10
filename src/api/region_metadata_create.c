#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t sm_region_metadata_create (dram_region_id_t id) {
   // Check argument validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure
   dram_region_t *r_ptr = &(SM_GLOBALS.regions[id]);

   if(!lock_acquire(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be free
   if(r_ptr->state != dram_region_free) {
      lock_release(r_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   // Set the DRAM region type and state
   r_ptr->type  = metadata_region;
   r_ptr->state = dram_region_owned;

   // Initialize the metadata page map
   metadata_page_map_t page_map = (metadata_page_map_t) REGION_BASE(id);

   uint64_t init_value = metadata_free;

   for(int i = 0; i < NUM_METADATA_PAGES_PER_REGION; i++) {
      page_map[i] = init_value;
   }

   lock_release(r_ptr->lock); // Release Lock

   return monitor_ok;
}
