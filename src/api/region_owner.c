#include <sm.h>

TODO

enclave_id_t sm_region_owner (dram_region_id_t id) {
   // Check argument validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure
   dram_region_t *r_ptr = &(SM_GLOBALS.regions[id]);

   if(!lock_acquire(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   enclave_id_t owner = r_ptr->owner;

   lock_release(r_ptr->lock); // Release Lock

   return owner;
}
