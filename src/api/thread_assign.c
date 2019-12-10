#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

api_result_t thread_assign (enclave_id_t enclave_id, thread_id_t thread_id) {

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Check that the enclave is initialized.
   if(!enclave->initialized) {
      return monitor_invalid_state;
   }

   // TODO: Check that enclave hasn't been killed

   dram_region_t * dram_region_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   api_result_t ret = ecall_allocate_thread(enclave_id, thread_id);

   releaseLock(dram_region_ptr->lock);
   return ret;
}
