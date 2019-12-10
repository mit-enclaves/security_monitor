#include <sm.h>

api_result_t sm_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs) {

   // Check if enclave_id is valid
   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata and aquire the lock
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   // Check that the enclave is initialized.
   if(!enclave->initialized) {
      lock_release(er_info->lock);
      return monitor_invalid_state;
   }

   lock_release(er_info->lock);

   // Get the curent core metadata and aquire its lock
   core_t *core = &(SM_GLOBALS.cores[read_csr(CSR_MHARTID)]);

   if(!lock_acquire(core->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Get a pointer to the DRAM region datastructure of the thread metadata and aquire the lock
   dram_region_t * tr_info = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   if(!lock_acquire(tr_info->lock)) {
      lock_release(core->lock);
      return monitor_concurrent_call;
   } // Acquire Lock

   thread_t *thread = (thread_t *) thread_id;

   // Check thread_id validity
   api_result_t ret = is_valid_thread(enclave_id, thread_id);

   if(ret != monitor_ok) {
      lock_release(core->lock);
      lock_release(tr_info->lock); // Release Lock
      return ret;
   }

   // Check that the thread is not already sheduled
   if(!lock_acquire(thread->is_scheduled)) {
      lock_release(core->lock);
      lock_release(tr_info->lock); // Release Lock
      return monitor_invalid_state;
   }

   bool aex = thread->aex_present;

   // Save untrusted state, initialize enclave state
   for(int i = 0; i < NUM_REGISTERS; i++) {
      thread->untrusted_state[i] = regs[i];
      regs[i] = aex ? thread->aex_state[i] : 0; // TODO: Init registers?
   }

   // Save untrusted sp, pc
   thread->untrusted_sp = thread->untrusted_state[2];
   thread->untrusted_pc = read_csr(CSR_MSEPC);

   if(aex) {
      thread->aex_present = false;
   }

   lock_release(tr_info->lock); // Release Lock

   // Update the core's metadata
   core->owner = enclave_id;
   core->has_enclave_schedule = true;
   core->cur_thread = thread_id;

   lock_release(core->lock);

   // Set the enclave sp and pc
   write_csr(CSR_MEPC, thread->entry_pc);

   uint64_t mstatus_tmp = read_csr(mstatus);

   // Set TVM to one, MPP to 0 (U mode), MPIE, SIE to 0 and UIE to 1
   mstatus_tmp |= MSTATUS_TVM_MASK;
   mstatus_tmp &= (~MSTATUS_MPP_MASK);
   mstatus_tmp &= (~MSTATUS_MPIE_MASK);
   mstatus_tmp &= (~MSTATUS_SIE_MASK);
   mstatus_tmp |= MSTATUS_UIE_MASK;

   write_csr(CSR_MSTATUS, mstatus_tmp);

   // Set sanctum specific CSRs
   swap_csr(CSR_MEATP, enclave->eptbr);

   swap_csr(CSR_MEVBASE, enclave->evbase);
   swap_csr(CSR_MEVMASK, enclave->evmask);

   swap_csr(CSR_MEMRBM, enclave->dram_bitmap);

   swap_csr(CSR_MEPARBASE, enclave->meparbase);
   swap_csr(CSR_MEPARMASK, enclave->meparmask);

   // Set trap handler
   swap_csr(CSR_MTVEC, thread->fault_pc);

   // Set sp
   write_csr(CSR_MSCRATCH, thread->entry_sp);
   asm volatile ("csrrw sp, mscratch, sp");
   swap_csr(CSR_MSCRATCH, thread->fault_sp);

   // Clean State

   clean_reg(x1);
   // Do not clean x2
   clean_reg(x3);
   clean_reg(x4);
   clean_reg(x5);
   clean_reg(x6);
   clean_reg(x7);
   clean_reg(x8);
   clean_reg(x9);
   clean_reg(x10);
   clean_reg(x11);
   clean_reg(x12);
   clean_reg(x13);
   clean_reg(x14);
   clean_reg(x15);
   clean_reg(x16);
   clean_reg(x17);
   clean_reg(x18);
   clean_reg(x19);
   clean_reg(x20);
   clean_reg(x21);
   clean_reg(x22);
   clean_reg(x23);
   clean_reg(x24);
   clean_reg(x25);
   clean_reg(x26);
   clean_reg(x27);
   clean_reg(x28);
   clean_reg(x29);
   clean_reg(x30);
   clean_reg(x31);

   // mret
   asm volatile ("mret");

   return monitor_ok; // TODO : useless
}
