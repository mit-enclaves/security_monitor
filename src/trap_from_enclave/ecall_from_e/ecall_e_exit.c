#include <ecall_e.h>
#include <sm.h>
#include <csr/csr.h>
#include <clib/clib.h> 
#include <sm_util/sm_util.h>

SM_ETRAP api_result_t ecall_exit_enclave(uintptr_t *regs) {

   core_t *core = &(sm_globals.cores[read_csr(mhartid)]);
   
   thread_id_t thread_id = core->cur_thread;

   enclave_id_t enclave_id = core->owner;

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
   
   thread->aex_present = false; // TODO: Usefull?

   releaseLock(tr_ptr->lock); // Release Lock

   // Clean the core's metadata
   core->owner = 0;
   core->has_enclave_schedule = false;
   core->cur_thread = 0;

   releaseLock(core->lock);

   // Set the enclave sp and pc
   write_csr(mepc, thread->untrusted_pc + 4);

   uint64_t mstatus_tmp = read_csr(mstatus);

   // Set TVM to 1, MPP to 1 (S mode), MPIE to 0, SIE to 1 and UIE to 0
   mstatus_tmp |= MSTATUS_TVM_MASK;
   mstatus_tmp &= (~MSTATUS_MPP_MASK);
   mstatus_tmp |= 1ul << MSTATUS_MPP_OFFSET;
   mstatus_tmp &= (~MSTATUS_MPIE_MASK);
   mstatus_tmp |= MSTATUS_SIE_MASK;
   mstatus_tmp &= (~MSTATUS_UIE_MASK);

   write_csr(mstatus, mstatus_tmp);

   // Set sanctum specific CSRs
   enclave_t *enclave = (enclave_t *) enclave_id;
   
   swap_csr(CSR_MEATP, enclave->eptbr);

   swap_csr(CSR_MEVBASE, enclave->evbase);
   swap_csr(CSR_MEVMASK, enclave->evmask);

   swap_csr(CSR_MEMRBM, enclave->dram_bitmap);

   swap_csr(CSR_MEPARBASE, enclave->meparbase);
   swap_csr(CSR_MEPARMASK, enclave->meparmask);

   // Set trap handler
   swap_csr(mtvec, thread->fault_pc);

   // TODO: Recover State

   // Set sp and mret
   write_csr(mscratch, thread->untrusted_sp);
   asm volatile ("csrrw sp, mscratch, sp");
   swap_csr(mscratch, thread->fault_sp);
   asm volatile ("mret");

   return monitor_ok; // TODO : useless
}
