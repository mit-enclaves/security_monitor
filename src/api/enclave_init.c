#include <sm.h>

api_result_t sm_enclave_init (enclave_id_t enclave_id) {

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      lock_release(er_info->lock);
      return monitor_invalid_state;
   }

   // Initialize Enclave
   enclave->initialized = true;

   // Output the measurement
   sha3_final(&(enclave->measurement),&(enclave->sha3_ctx));

   lock_release(er_info->lock);

   return monitor_ok;
}
