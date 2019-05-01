#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

dram_region_state_t dram_region_state(dram_region_id_t id) {
   // Check argument validity
   if(id < NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure	
   dram_region_t *r_ptr = &(sm_globals.regions[id]);

   if(!aquireLock(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   dram_region_type_t state = r_ptr->state;

   releaseLock(r_ptr->lock); // Release Lock

   return state; 
}

enclave_id_t dram_region_owner(dram_region_id_t id) {
   // Check argument validity
   if(id < NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure	
   dram_region_t *r_ptr = &(sm_globals.regions[id]);

   if(!aquireLock(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   enclave_id_t owner = r_ptr->owner;

   releaseLock(r_ptr->lock); // Release Lock

   return owner; 
}

api_result_t assign_dram_region(dram_region_id_t id, enclave_id_t new_owner) {
   // Check arguments validity
   if(id < NUM_REGIONS) {
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
      dram_region_t *n_ow_ptr = &(sm_globals.regions[REGION_IDX(new_owner)]);

      if(!aquireLock(n_ow_ptr->lock)) {
         return monitor_concurrent_call;
      }
      
      // Check that new_owner (if an enclave) is not initialized.
      if(((enclave_t *) new_owner)->initialized) {
         releaseLock(n_ow_ptr->lock);
         return monitor_invalid_state;
      }

      releaseLock(n_ow_ptr->lock);
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

   // Set the new owner and update the owner's bitmap
   if(is_enclave){
      // Get a pointer to the DRAM region datastructure of the new_owner
      dram_region_t *n_ow_ptr = &(sm_globals.regions[REGION_IDX(new_owner)]);

      if(!aquireLock(n_ow_ptr->lock)) {
         return monitor_concurrent_call;
      }
      
      ((enclave_t *) new_owner)->dram_bitmap |= (1ul << id);
      
      releaseLock(n_ow_ptr->lock);
      
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

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}

api_result_t os_block_dram_region(dram_region_id_t id) {
   // Check argument validity
   if(id < NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure	
   dram_region_t *r_ptr = &(sm_globals.regions[id]);

   if(!aquireLock(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be owned
   if(r_ptr->state != dram_region_owned) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   // This handler only handle OS-owned regions
   if((r_ptr->type != untrusted_region)) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   // Update the DRAM region state
   r_ptr->state = dram_region_blocked;

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}

api_result_t free_dram_region(dram_region_id_t id) {
   // Check argument validity
   if(id < NUM_REGIONS) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure	
   dram_region_t *r_ptr = &(sm_globals.regions[id]);

   if(!aquireLock(r_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // The DRAM region must be blocked
   if(r_ptr->state != dram_region_blocked) {
      releaseLock(r_ptr->lock); // Release Lock
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

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}
