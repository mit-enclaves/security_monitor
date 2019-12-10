#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

dram_region_state_t sm_region_state (dram_region_id_t id) {
   // Check argument validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure
   dram_region_t *r_ptr = &(SM_GLOBALS.regions[id]);

   if(!lock_acquire(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   dram_region_type_t state = r_ptr->state;

   lock_release(r_ptr->lock); // Release Lock

   return state;
}
