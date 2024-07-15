#include <sm.h>

typedef uint64_t bases_t[NUM_REGIONS];

api_result_t sm_internal_region_cache_partitioning ( cache_partition_t *part ) {
    // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - The caller must be the operating system
    - The partitionning must be valid (the sum must be equal to the size of the cache)
    - The SM regions should not be touched
  */

  sm_state_t *sm = get_sm_state_ptr();
  int core_id = platform_get_core_id();

  if(!lock_untrusted_state()) {
    return MONITOR_CONCURRENT_CALL;
  }

  // Check that the partitioning is correct (the sum must be equal to the size of the cache)
  cache_partition_t copy;
  memcpy_u2m(&copy, part, sizeof(cache_partition_t));

  uint64_t sum = 0;
  uint64_t found_first_mod = false;
  uint64_t idx_first_mod = 0;
  bases_t bases = {0};
  for(int i = 0; i < NUM_REGIONS; i++) {
    if(copy.lgsizes[i] >= 10) {
      unlock_untrusted_state();
      return MONITOR_INVALID_VALUE;
    }
    if(!found_first_mod) {
      if(copy.lgsizes[i] != sm->llc_partitions.lgsizes[i]) {
        found_first_mod = true;
      } else {
        idx_first_mod++;
      }
    }
    uint64_t size = 1 << copy.lgsizes[i];
    bases[i] = sum;
    sum += size;
  }

  if(sum > 1024) {
    unlock_untrusted_state();
    return MONITOR_INVALID_VALUE;
  }

  // Identify the lowest region modified
  // Check that the first SM region is not modified (metadata regions are not touched during the change)
  if(idx_first_mod <= 0) {
    unlock_untrusted_state();
    return MONITOR_INVALID_VALUE; 
  }

  // Set up LLC Sync datastructure
  while(!platform_lock_acquire(&sm->llc_sync.lock));
  if(sm->llc_sync.busy == true) {
    platform_lock_release(&sm->llc_sync.lock);
    unlock_untrusted_state();
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

  // Once everyone has reached the waiting state, check that no enclave is running.
  bool no_enclaves_running = true;
  for(int i = 0; i < NUM_CORES; i++) {
    assert(lock_core(i));
    if(sm->cores->owner != OWNER_UNTRUSTED) {
      no_enclaves_running = false;
      break;
    }
    unlock_core(i);
  }

  // If no enclave is running... 
  if(no_enclaves_running) {
    // Flush the LLC regions
    for(int rid = idx_first_mod; rid < NUM_REGIONS; rid++) {
      flush_llc_region(rid);
    };

    // Send intructions to change the partitioning starting at the highest modified region
    uint64_t *llcCtrl = (uint64_t *) LLC_CTRL_ADDR;
    for(int rid = idx_first_mod; rid < NUM_REGIONS; rid++) {
      uint64_t base = bases[rid];
      uint64_t size = copy.lgsizes[rid];
      printm("Setting up LLC slice %d with base %x and size %x\n", rid, base, size);
      *llcCtrl = (rid << LLC_CTRL_ID_OFFSET) + (base << LLC_CTRL_BASE_OFFSET) + (size << LLC_CTRL_SIZE_OFFSET);
      sm->llc_partitions.lgsizes[rid] = size;
    }
  }

  // Clean up the LLC Sync datastructure
  while(!platform_lock_acquire(&sm->llc_sync.lock));
  sm->llc_sync.waiting = 0;
  sm->llc_sync.wait = false;
  sm->llc_sync.left = 1;
  asm volatile ("fence");
  platform_lock_release(&sm->llc_sync.lock);

  unlock_untrusted_state();
  
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

  if(no_enclaves_running) {
    return MONITOR_OK;
  } else {
    return MONITOR_INVALID_STATE;
  }
}