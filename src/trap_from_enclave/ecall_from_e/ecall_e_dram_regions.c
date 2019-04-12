#include <api.h>
#include <sm.h>
#include <csr/csr.h>

api_result_t enclave_block_dram_region(dram_region_id_t id) {
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

   // Check that the caller is the owner
   if(r_ptr->type != enclave_region) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;

   if(caller_id != r_ptr->owner) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   // Update the DRAM region state
   r_ptr->state = dram_region_blocked;

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}


api_result_t dram_region_check_ownership(dram_region_id_t id) {
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

   // Check that the caller is the owner
   if(r_ptr->type != enclave_region) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;

   if(caller_id != r_ptr->owner) {
      releaseLock(r_ptr->lock); // Release Lock
      return monitor_access_denied;
   }

   releaseLock(r_ptr->lock); // Release Lock

   return monitor_ok;
}

