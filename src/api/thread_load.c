#include <sm.h>

api_result_t sm_internal_thread_load (enclave_id_t enclave_id, thread_id_t thread_id,
  uintptr_t entry_pc, uintptr_t entry_stack, uint64_t timer_limit) {

  // Validate inputs
  // ---------------

  /*
   - enclave_id must be valid
   - enclave must be ENCLAVE_STATE_PAGE_DATA_LOADED
   - thread_id must point to a free metadata region
   - entry_pc must point to a region owned by the enclave
   - entry_stack must point to a region owned by the enclave
  */
  
  /*
   - timer_limit is covered by measurement.
  */

  // Lock the enclave's metadata's region and the thread_id region (if different)

  region_map_t locked_regions = (const region_map_t){ 0 };

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  uint64_t region_id_thread = addr_to_region_id(thread_id);
  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;

  uint64_t num_metadata_pages = thread_metadata_pages();

  // <TRANSACTION>
  // enclave_id must be valid
  // Lock the enclave's metadata's region
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  // enclave must be in state ENCLAVE_STATE_PAGE_DATA_LOADED
  if(enclave_metadata->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // thread_id must point to a free metadata region
  // Lock the thread_id region (if different)
  result = add_lock_region_iff_free_metadata_pages(thread_id, num_metadata_pages, &locked_regions);
  if ( MONITOR_OK != result ) {
    unlock_regions(&locked_regions);
    return result;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Allocate the thread's metadata regions
  metadata_region_t * region = region_id_to_addr(region_id_thread);
  uint64_t page_id = addr_to_region_page_id(thread_id);

  region->page_info[page_id] = METADATA_PAGE_THREAD;
  for(int i = 1; i < num_metadata_pages; i++) {
    region->page_info[page_id + i] = METADATA_PAGE_INVALID;
  }

  thread_metadata->owner        = enclave_id;
  //thread_metadata->is_schedule  = 0;
  //thread_metadata->aex_present  = false;
  //thread_metadata->untrusted_pc = 0;
  thread_metadata->entry_pc     = entry_pc;
  thread_metadata->entry_sp     = entry_stack;
  thread_metadata->timer_limit  = timer_limit;

  /*
  for(int i = 0; i < NUM_REGISTERS; i++) {
    thread_metadata->untrusted_state[i] = 0;
    thread_metadata->fault_state[i] = 0;
    thread_metadata->aex_state[i]   = 0;
  }
  */

  // Increment the enclave's thread counter
  enclave_metadata->num_threads++;

  // Hash the arguments into enclave state
  hash_extend(&enclave_metadata->hash_context, &entry_pc, sizeof(entry_pc));
  hash_extend(&enclave_metadata->hash_context, &entry_stack, sizeof(entry_stack));
  hash_extend(&enclave_metadata->hash_context, &timer_limit, sizeof(timer_limit));
  
  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
