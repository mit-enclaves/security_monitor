#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t sm_region_block (dram_region_id_t id) {
   // Check argument validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure
   dram_region_t *r_ptr = &(SM_GLOBALS.regions[id]);

   if(!lock_acquire(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be owned
   if(r_ptr->state != dram_region_owned) {
      lock_release(r_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   // This handler only handle OS-owned regions
   if((r_ptr->type != untrusted_region)) {
      lock_release(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   // Update the DRAM region state
   r_ptr->state = dram_region_blocked;

   lock_release(r_ptr->lock); // Release Lock

   return monitor_ok;
}
