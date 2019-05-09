#include <ecall_e.h>
#include <sm.h>
#include <csr/csr.h>
#include <clib/clib.h> 
#include <sm_util/sm_util.h>

SM_ETRAP api_result_t ecall_accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr) {

   // Get the caller id
   enclave_id_t caller_id = SM_GLOBALS.cores[read_csr(mhartid)].owner;
   
   dram_region_t * tr_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(tr_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
   
   // Check thread_id validity   
   api_result_t ret = is_valid_thread(caller_id, thread_id);

   if(ret != monitor_ok) {
      releaseLock(tr_ptr->lock); // Release Lock
      return ret;
   }

   // TODO: How to check that this thread is not already initialized?

   // TODO: Check thread_info_t validity?
   
   // Copy the metadata
   memcpy((void *) thread_id, (void *) thread_info_addr, sizeof(thread_t));

   releaseLock(tr_ptr->lock); // Release Lock
   
   // Increase the enclave's thread_count
   dram_region_t *er_ptr = &(SM_GLOBALS.regions[REGION_IDX(caller_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   ((enclave_t *) caller_id)->thread_count++;

   releaseLock(er_ptr->lock);

   return monitor_ok;
}
