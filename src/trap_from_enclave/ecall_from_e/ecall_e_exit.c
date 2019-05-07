#include <ecall_e.h>
#include <sm.h>
#include <csr/csr.h>
#include <clib/clib.h> 
#include <sm_util/sm_util.h>

SM_ETRAP api_result_t ecall_exit_enclave(uintptr_t *regs) {

   core_t *core = &(sm_globals.cores[read_csr(mhartid)]);
   
   thread_id_t thread_id = core->cur_thread;

   // Aquire current core's metadata lock
   if(!aquireLock(core->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
   
   dram_region_t * tr_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);
   
   if(!aquireLock(tr_ptr->lock)) {
      releaseLock(core->lock);
      return monitor_concurrent_call;
   } // Acquire Lock
   
   thread_t *thread = (thread_t *) thread_id;

   // Re-establish untrusted state
   for(int i = 0; i < NUM_REGISTERS; i++) {
      regs[i] = thread->untrusted_state[i];
   }
   
   // Set the OS pc
   write_csr(mepc, thread->untrusted_pc);

   thread->aex_present = false; // TODO: Usefull?

   releaseLock(tr_ptr->lock); // Release Lock

   // Clean the core's metadata
   core->owner = 0;
   core->has_enclave_schedule = false;
   core->cur_thread = 0;

   releaseLock(core->lock);

   return monitor_ok;
}
