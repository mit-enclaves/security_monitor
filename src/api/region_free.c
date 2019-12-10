#include <sm.h>

api_result_t sm_region_free (dram_region_id_t id) {
   // Check argument validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure
   dram_region_t *r_ptr = &(SM_GLOBALS.regions[id]);

   if(!lock_acquire(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be blocked
   if(r_ptr->state != dram_region_blocked) {
      lock_release(r_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   // If the DRAM region belongs to the OS
   // remove it from the OS bitmap
   if(r_ptr->type == untrusted_region) {
      XLENINT mmrbm = read_csr(CSR_MMRBM);
      mmrbm &= ~(1ul << id);
      write_csr(CSR_MMRBM, mmrbm);
   }
   // If the DRAM region belongs to an enclave
   // remove it from the enclave bitmap
   else if(r_ptr->type == enclave_region) {
      XLENINT memrbm = ((enclave_t *) r_ptr->owner)->dram_bitmap;
      memrbm &= ~(1ul << id);
      ((enclave_t *) r_ptr->owner)->dram_bitmap = memrbm;
   }

   // Update the DRAM region state
   r_ptr->state = dram_region_free;

   lock_release(r_ptr->lock); // Release Lock

   return monitor_ok;
}
