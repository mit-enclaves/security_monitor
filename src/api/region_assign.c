#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t sm_region_assign (dram_region_id_t id, enclave_id_t new_owner) {
   // Check arguments validity
   if(id >= NUM_REGIONS) {
      return monitor_invalid_value;
   }

   bool is_enclave = (new_owner != 0);
   // Check new_owner is a valid enclave
   if(is_enclave && !is_valid_enclave(new_owner)){
      return monitor_invalid_value;
   }

   // Check that new_owner is not initialized
   if(is_enclave) {
      // Get a pointer to the DRAM region datastructure of the new_owner
      dram_region_t *n_ow_ptr = &(SM_GLOBALS.regions[REGION_IDX(new_owner)]);

      if(!lock_acquire(n_ow_ptr->lock)) {
         return monitor_concurrent_call;
      }

      // Check that new_owner (if an enclave) is not initialized.
      if(((enclave_t *) new_owner)->initialized) {
         lock_release(n_ow_ptr->lock);
         return monitor_invalid_state;
      }

      lock_release(n_ow_ptr->lock);
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

   // Set the new owner and update the owner's bitmap
   if(is_enclave){
      // Get a pointer to the DRAM region datastructure of the new_owner
      dram_region_t *n_ow_ptr = &(SM_GLOBALS.regions[REGION_IDX(new_owner)]);

      if(!lock_acquire(n_ow_ptr->lock)) {
         return monitor_concurrent_call;
      }

      ((enclave_t *) new_owner)->dram_bitmap |= (1ul << id);

      lock_release(n_ow_ptr->lock);

      r_ptr->owner = new_owner;
      r_ptr->type  = enclave_region;
   }
   else{
      XLENINT mmrbm = read_csr(CSR_MMRBM);
      mmrbm |= (1ul << id);
      write_csr(CSR_MMRBM, mmrbm);

      r_ptr->owner = 0;
      r_ptr->type  = untrusted_region;
   }

   // Update the DRAM region state
   r_ptr->state = dram_region_owned;

   lock_release(r_ptr->lock); // Release Lock

   return monitor_ok;
}
