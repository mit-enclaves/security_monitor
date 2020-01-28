#include <sm.h>

extern uintptr_t trap_vector_from_untrusted;
extern uintptr_t stack_ptr;

api_result_t sm_internal_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs) {

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

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  // enclave_id must be valid
  // Lock the enclave's metadata's region
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  // enclave must be in state ENCLAVE_STATE_PAGE_DATA_LOADED
  if(enclave_metadata->init_state != ENCLAVE_STATE_INITIALIZED) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // thread_id must be valid
  result = add_lock_region_iff_valid_thread(thread_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  // the tread must not be scheduled
  if(thread_metadata->is_scheduled) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // Get the curent core's lock
  if(!lock_core(core_id)) {
    unlock_regions(&locked_regions);
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

  // Save untrusted pc
  thread_metadata->untrusted_pc = read_csr(mepc);

  // Save untrusted fault handler pc and fault sp
  thread_metadata->untrusted_fault_pc = (((uint64_t)(&trap_vector_from_untrusted))&(~0x3L));
  thread_metadata->untrusted_fault_sp = stack_ptr + (core_id * (STACK_SIZE));

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

  // Prepare enclave pc
  write_csr(mepc, thread_metadata->entry_pc);
  // Prepare enclave sp
  swap_csr(mscratch, thread_metadata->fault_sp);

  // Release locks
  unlock_regions(&locked_regions);
  unlock_core(core_id);
  // </TRANSACTION>

  register uint64_t t0 asm ("t0") = thread_metadata->entry_sp;
  asm volatile (" \
  mv sp, t0; \n \
  call platform_clean_core; \n \
  call platform_purge_core; \n \
  li ra, 0; \n \
  mret"  : : "r" (t0));

  return MONITOR_OK; // Not rechable
}
