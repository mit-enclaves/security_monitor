#include <sm.h>

#error not implemented

TODO lock the thread metadata region iff free pages available

Lock the thread's enclave metadata region (if different)

If the enclave is in incorrect state, error (should be page loaded)

entry pc should be in owned region

entry stack should be in owned region

fault pc should be in owned region

fault stack should be in owned region

---

Create the thread data structure

Hahsh the arguments into enclave state

Increment the enclave's thread counter

api_result_t sm_thread_load (enclave_id_t enclave_id, thread_id_t thread_id,
    uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc,
    uintptr_t fault_stack) {

   // TODO check other arguments validity

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_metadata_t * enclave = (enclave_metadata_t *) enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   dram_region_t * dram_region_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   api_result_t ret = ecall_allocate_thread(enclave_id, thread_id);

   if(ret != monitor_ok) {
      releaseLock(dram_region_ptr->lock);
      return ret;
   }

   thread_t *thread = (thread_t *) thread_id;

   thread->is_scheduled.flag   = 0;
   thread->aex_present    = false;
   thread->untrusted_pc   = 0;
   thread->untrusted_sp   = 0;
   thread->entry_pc       = entry_pc;
   thread->entry_sp       = entry_stack;
   thread->fault_pc       = fault_pc;
   thread->fault_sp       = fault_stack;

   for(int i = 0; i < NUM_REGISTERS; i++) {
      thread->fault_state[i] = 0;
      thread->aex_state[i]   = 0;
   }

   releaseLock(dram_region_ptr->lock); // Release Lock

   // Increase the enclave's thread_count
   dram_region_t *er_ptr = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   enclave->thread_count++;

   // Update measurement
   struct inputs_load_thread_t inputs = {0};
   inputs.entry_pc = entry_pc;
   inputs.entry_stack = entry_stack;
   inputs.fault_pc = fault_pc;
   inputs.fault_stack = fault_stack;

   sha3_update(&(enclave->sha3_ctx), &(inputs), sizeof(struct inputs_load_thread_t));

   releaseLock(er_ptr->lock);

   return monitor_ok;
}
