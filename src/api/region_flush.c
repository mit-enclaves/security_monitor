#include <sm.h>

api_result_t sm_internal_region_flush ( region_id_t id ) {
// Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

// Validate inputs
// ---------------

/*
- region_id must be valid
- the region lock should be available
- the owner must be OWNER_UNTRUSTED
*/

  // Check that the input is correct
  if(id < 0 || id > NUM_REGIONS) {
    return MONITOR_INVALID_VALUE;
  }

  sm_state_t *sm = get_sm_state_ptr();
  int core_id = platform_get_core_id();

  // Grab the region lock
  if(!lock_region(id)) {
    return MONITOR_CONCURRENT_CALL;
  }

  // Check that the region is not owned by the caller (only untrusted for now)
  if(sm->regions[id].owner != OWNER_UNTRUSTED) {
    unlock_region(id);
    return MONITOR_INVALID_STATE; 
  }

  // Set up LLC Sync datastructure
  while(!platform_lock_acquire(&sm->llc_sync.lock));
  if(sm->llc_sync.busy == true) {
    platform_lock_release(&sm->llc_sync.lock);
    unlock_region(id);
    return MONITOR_CONCURRENT_CALL;
  }
  sm->llc_sync.waiting = 1;
  sm->llc_sync.wait = true;
  sm->llc_sync.left = 0;
  sm->llc_sync.busy = true;
  platform_lock_release(&sm->llc_sync.lock);
  
  // Send IPI to all cores to interupt them
  for(int i = 0; i < NUM_CORES; i++) {
    if(i == core_id) {
      continue;
    } else {
      send_ipi(i, IPI_SOFT);
    }
  }
  
  // Wait for everyone to be waiting
  int waiting;
  do {
    while(!platform_lock_acquire(&sm->llc_sync.lock));
    waiting = sm->llc_sync.waiting;
    platform_lock_release(&sm->llc_sync.lock); 
  } while(waiting < NUM_CORES);

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Once everyone has reached the waiting state, flush the region
  flush_llc_region(id);

  // Release lock and return
  unlock_region(id);

  // Clean up the LLC Sync datastructure
  while(!platform_lock_acquire(&sm->llc_sync.lock));
  sm->llc_sync.waiting = 0;
  sm->llc_sync.wait = false;
  sm->llc_sync.left = 1;
  asm volatile ("fence");
  platform_lock_release(&sm->llc_sync.lock);

  // Wait for everyone to have left
  int left;
  do {
    while(!platform_lock_acquire(&sm->llc_sync.lock));
    left = sm->llc_sync.left;
    platform_lock_release(&sm->llc_sync.lock); 
  } while(left < NUM_CORES);

  // Release LLC Sync
  while(!platform_lock_acquire(&sm->llc_sync.lock));
  sm->llc_sync.busy = false;
  asm volatile ("fence");
  platform_lock_release(&sm->llc_sync.lock);

  return MONITOR_OK;
}