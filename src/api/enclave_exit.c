#include <sm.h>

api_result_t sm_enclave_exit() { // TODO: noreturn

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*

  */

  // Get the core's lock
  // Update core's metadata
  // Lock thread's metadata region
  // Throw away current context
  // Restore untrusted CPU context
  // Unlock thread
  // Resume untrusted context

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  sm_state_t * sm = get_sm_state_ptr();

  uint64_t core_id = read_csr(mhartid);
  sm_core_t *core_metadata = &(sm->cores[core_id]);

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  // Get the curent core's lock
  if(!lock_core(core_id)) {
    return MONITOR_CONCURRENT_CALL;
  }

  enclave_id_t enclave_id = core_metadata->owner;
  uint64_t region_id_enclave = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  thread_id_t thread_id = core_metadata->thread;
  uint64_t region_id_thread = addr_to_region_id(thread_id);
  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;


  // Clean the core's metadata
  core_metadata->owner = OWNER_UNTRUSTED;
  core_metadata->thread = 0;

  unlock_core(core_id);
  // <\TRANSACTION>


  // <TRANSACTION>
  // Lock the thread's metadata region
  api_result_t result = add_lock_region(region_id_enclave, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  result = add_lock_region(region_id_thread, &locked_regions);
  if ( MONITOR_OK != result ) {
    unlock_regions(&locked_regions);
    return result;
  }

  // Set MSTATUS
  uint64_t mstatus_tmp = read_csr(mstatus);

  // Set TVM to 1, MPP to 1 (S mode), MPIE to 0, SIE to 1 and UIE to 0
  mstatus_tmp |= MSTATUS_TVM_MASK;
  mstatus_tmp &= (~MSTATUS_MPP_MASK);
  mstatus_tmp |= 1ul << MSTATUS_MPP_OFFSET;
  mstatus_tmp &= (~MSTATUS_MPIE_MASK);
  mstatus_tmp |= MSTATUS_SIE_MASK;
  mstatus_tmp &= (~MSTATUS_UIE_MASK);

  write_csr(mstatus, mstatus_tmp);

  // Swap the page table root
  swap_csr(satp, enclave_metadata->eptbr);

  // Restore trap handler
  swap_csr(mtvec, thread_metadata->fault_pc);

  // Set pc
  write_csr(mepc, thread_metadata->untrusted_pc + 4);
  // Set sp
  write_csr(mscratch, thread_metadata->untrusted_sp);
  asm volatile ("csrrw sp, mscratch, sp");
  swap_csr(mscratch, thread_metadata->fault_sp);

  // TODO: Recover Register File
  thread_metadata->aex_present = false; // TODO: Usefull?

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  // Procede to mret
  asm volatile ("mret");

  return MONITOR_OK; // Unreachable
}
