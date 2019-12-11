#include <sm.h>

api_result_t sm_enclave_load_handler (enclave_id_t enclave_id) {
   // TODO: Does phys_addr has to be alligned?

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   if(!is_valid_enclave(enclave_id)) {
      lock_release(er_info->lock);
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *)  enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      lock_release(er_info->lock);
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(enclave->last_phys_addr_loaded > phys_addr) {
      lock_release(er_info->lock);
      return monitor_invalid_value;
   }

   // Check that phys_addr points into a DRAM region owned by the enclave
   if(!owned(phys_addr, enclave_id)){
      lock_release(er_info->lock);
      return monitor_invalid_state;
   }

   dram_region_t *r_info = &(SM_GLOBALS.regions[REGION_IDX(phys_addr)]);

   // Check that the handlers fit in a DRAM region owned by the enclave
   uint64_t size_handler = ((uint64_t) &_enclave_trap_handler_end) - ((uint64_t) &_enclave_trap_handler_start);

   uintptr_t end_phys_addr = phys_addr + size_handler;

   dram_region_t *r_end_info = NULL;

   // TODO: Make sure the handlers are not larger than a DRAM region
   if(REGION_IDX(phys_addr) != REGION_IDX(end_phys_addr)){
      // Check that end_phys_addr points into a DRAM region owned by the enclave
      if(!owned(end_phys_addr, enclave_id)){
         lock_release(er_info->lock);
         return monitor_invalid_state;
      }
      r_end_info = &(SM_GLOBALS.regions[REGION_IDX(end_phys_addr)]);
   }

   enclave->meparbase = phys_addr;
   enclave->meparmask = ~((1ul << (intlog2(end_phys_addr - phys_addr) + 1)) - 1);

   // TODO: need to grab the DRAM regions locks

   if(!lock_acquire(r_info->lock)) {
      lock_release(er_info->lock);
      return monitor_concurrent_call;
   }

   if(r_end_info != NULL) {
      if(!lock_acquire(r_end_info->lock)) {
         lock_release(er_info->lock);
         lock_release(r_info->lock);
         return monitor_concurrent_call;
      }
   }


   // Copy the handlers
   memcpy((void *) phys_addr, (void *) &_enclave_trap_handler_start, size_handler);

   // Update the measurement
   sha3_update(&(enclave->sha3_ctx), (void *) &_enclave_trap_handler_start, size_handler);

   lock_release(er_info->lock);
   lock_release(r_info->lock);
   if(r_end_info != NULL) {
      lock_release(r_end_info->lock);
   }

   return monitor_ok;
}
