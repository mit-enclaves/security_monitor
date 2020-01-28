#include <sm.h>

api_result_t sm_internal_thread_delete (thread_id_t thread_id) {

  // Validate inputs
  // ---------------

  /*
    - thread_id must be valid
    - the tread must not be scheduled
  */

  // Note, the thread owner is deemed valid

  // Lock the thread_id region


  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  // thread_id must be valid
  api_result_t result = add_lock_region_iff_valid_thread(thread_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  uint64_t region_id_thread = addr_to_region_id(thread_id);
  thread_metadata_t *thread_metadata = (thread_metadata_t *) thread_id;

  enclave_id_t enclave_id = thread_metadata->owner;
  uint64_t region_id_enclave = addr_to_region_id(enclave_id);
  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);


  // Lock the thread's owner/enclave metadata region
  if(!add_lock_region(region_id_enclave, &locked_regions)) {
    unlock_regions(&locked_regions);
    return MONITOR_CONCURRENT_CALL;
  }

  // the tread must not be scheduled
  if(thread_metadata->is_scheduled) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Decrement the enclave's thread counter
  enclave_metadata->num_threads--;

  // Erase the thread data structure
  memset((void *) thread_id,  0x00, sizeof(thread_metadata_t));

  // Clean the metadata page map
  metadata_region_t * region = region_id_to_addr(region_id_thread);

  uint64_t num_metadata_pages = thread_metadata_pages();
  uint64_t page_id = addr_to_region_page_id(thread_id);

  for(int i = 0; i < num_metadata_pages; i++) {
    region->page_info[page_id + i] = METADATA_PAGE_FREE;
  }

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
