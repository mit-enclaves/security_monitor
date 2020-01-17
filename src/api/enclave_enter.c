#include <sm.h>

api_result_t sm_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs) {

  //TODO: lock enclave metadata region with add_locked_region
  //TODO: lock thread metadata region with add_locked_region

  //TODO: thread_metadata->is_scheduled = true

  //TODO: release locks with unlock_regions

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
    - the enclave must be in state ENCLAVE_STATE_INITIALIZED
    - thread_id must be valid
    - the thread must not be scheduled
  */

  // Lock the enclave and thread's metadata region (if different)
  // Lock the current core's lock
  // Save untrusted state, initialize enclave state
  // Save untrusted sp, pc
  // Update the core's metadata
  // Prepapre the core
  // Perform a mret

  sm_state_t * sm = get_sm_state_ptr();

  uint64_t core_id = read_csr(mhartid);
  sm_core_t *core_metadata = &(sm->cores[core_id]);

  uint64_t region_id_enclave = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  uint64_t region_id_thread = addr_to_region_id(thread_id);
  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;

  // <TRANSACTION>
  // enclave_id must be valid
  // Lock the enclave's metadata's region
  api_result_t result = lock_region_iff_valid_enclave( enclave_id );
  if ( MONITOR_OK != result ) {
    return result;
  }

  // enclave must be in state ENCLAVE_STATE_PAGE_DATA_LOADED
  if(enclave_metadata->init_state != ENCLAVE_STATE_INITIALIZED) {
    unlock_region(region_id_enclave);
    return MONITOR_INVALID_STATE;
  }

  // thread_id must be valid
  // Lock the thread_id region (if different)
  bool different_region = (region_id_thread != region_id_enclave);
  bool not_locked = different_region; // If the regions are different, we need to lock it
  result = lock_region_iff_valid_thread_and_not_locked(thread_id, not_locked);
  if ( MONITOR_OK != result ) {
    return result;
  }

  // the tread must not be scheduled
  if(thread_metadata->is_scheduled) {
    unlock_region(region_id_enclave);
    unlock_region(region_id_thread);
    return MONITOR_INVALID_STATE;
  }

  // Get the curent core's lock
  if(!lock_core(core_id)) {
    unlock_region(region_id_enclave);
    unlock_region(region_id_thread);
    return MONITOR_CONCURRENT_CALL;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------


  //// Update the thread's metadata

  bool aex = thread_metadata->aex_present;

  // Save untrusted state, initialize enclave state
  for(int i = 0; i < NUM_REGISTERS; i++) {
    thread_metadata->untrusted_state[i] = regs[i];
    regs[i] = aex ? thread_metadata->aex_state[i] : 0; // TODO: Init registers?
  }

  // Save untrusted sp, pc
  thread_metadata->untrusted_sp = thread_metadata->untrusted_state[2];
  thread_metadata->untrusted_pc = read_csr(mepc);

  if(aex) {
    thread_metadata->aex_present = false;
  }


  //// Update the core's metadata

  core_metadata->owner = enclave_id;
  core_metadata->thread = thread_id;


  //// Prepare the core and mret

  // Update MSTATUS
  uint64_t mstatus_tmp = read_csr(mstatus);

  // Set TVM to one, MPP to 0 (U mode), MPIE, SIE to 0 and UIE to 1
  mstatus_tmp |= MSTATUS_TVM_MASK;
  mstatus_tmp &= (~MSTATUS_MPP_MASK);
  mstatus_tmp &= (~MSTATUS_MPIE_MASK);
  mstatus_tmp &= (~MSTATUS_SIE_MASK);
  mstatus_tmp |= MSTATUS_UIE_MASK;

  write_csr(mstatus, mstatus_tmp);

  // Swap the page table root
  swap_csr(satp, enclave_metadata->eptbr);

  // Setup the platform's memory protection mechanisms
  platform_protect_memory_enter_enclave(enclave_metadata);

  // Set trap handler
  swap_csr(mtvec, thread_metadata->fault_pc);

  // Set pc
  write_csr(mepc, thread_metadata->entry_pc);
  // Set sp
  write_csr(mscratch, thread_metadata->entry_sp);
  asm volatile ("csrrw sp, mscratch, sp");
  swap_csr(mscratch, thread_metadata->fault_sp);

  // Release locks
  unlock_region(region_id_enclave);
  unlock_region(region_id_thread);
  unlock_core(core_id);
  // </TRANSACTION>

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

  return MONITOR_OK; // Not rechable
}
