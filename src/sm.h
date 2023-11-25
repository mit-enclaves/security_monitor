#ifndef SECURITY_MONITOR_H
#define SECURITY_MONITOR_H

#include "sm_types.h"
#include "sm_constants.h"
#include <clib/clib.h>
#include <kernel/kernel_api.h>
#include <platform.h>

// Helpful macros
// --------------
/* We don't need this anymore probably
#define get_abs_addr(symbol) ({  \
  void * __tmp; \
  asm volatile ( \
    "lui %0, %%hi(" #symbol ") \n" \
    "addi %0, %0, %%lo(" #symbol ") \n" \
    : "=r"(__tmp)); \
  __tmp; \
})
*/

// Extern symbols defined in assembly

extern uintptr_t enclave_handler_start;
extern uintptr_t enclave_handler_end;

// Common minor operations
// -----------------------

// Global state accessors
static inline sm_state_t * get_sm_state_ptr (void) {
  return (sm_state_t *)SM_STATE_ADDR;
}


// Region helpers
enclave_id_t region_owner (region_id_t region_id);
enclave_id_t region_owner_lock_free (region_id_t region_id);

static inline region_id_t addr_to_region_id (uintptr_t addr) {
  return ((addr-RAM_BASE) & REGION_MASK) >> REGION_SHIFT; // will return an illegally large number in case of an address outside RAM. CAUTION!
}

static inline void * region_id_to_addr (uint64_t region_id) {
  return (void *)(RAM_BASE + (region_id << REGION_SHIFT));
}

static inline bool is_valid_region_id (uint64_t region_id) {
  return (region_id < NUM_REGIONS);
}

static inline bool is_page_aligned (uintptr_t addr) {
  return ((addr & PAGE_MASK) == 0);
}

static inline uint64_t addr_to_region_page_id (uintptr_t addr) {
  return (((addr-RAM_BASE) & (~REGION_MASK)) >> PAGE_SHIFT);
}

static inline bool is_valid_page_id_in_region (uint64_t page_id) {
  return page_id < NUM_REGION_PAGES;
}


// Synchronization helpers
static inline bool lock_untrusted_state () {
  sm_state_t * sm = get_sm_state_ptr();
  return platform_lock_acquire(&sm->untrusted_state_lock);
}

static inline void unlock_untrusted_state () {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_release(&sm->untrusted_state_lock);
}

static inline bool lock_core (uint64_t core_id) {
  sm_state_t * sm = get_sm_state_ptr();
  return platform_lock_acquire(&sm->cores[core_id].lock);
}

static inline void unlock_core (uint64_t core_id) {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_release(&sm->cores[core_id].lock);
}

static inline bool lock_region (region_id_t region_id) {
  sm_state_t * sm = get_sm_state_ptr();
  return platform_lock_acquire(&sm->regions[region_id].lock);
}

static inline void unlock_region (region_id_t region_id) {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_release(&sm->regions[region_id].lock);
}

static inline void unlock_regions (region_map_t * locked_regions) {
  for (int i=0; i<NUM_REGIONS; i++) {
    if (locked_regions->flags[i]) {
      unlock_region(i);
    }
  }
}

static inline bool add_lock_region (region_id_t region_id, region_map_t * locked_regions) {
  if (locked_regions->flags[region_id]) {
    return true;
  } else if ( lock_region(region_id) ) {
    locked_regions->flags[region_id] = true;
    return true;
  } else {
    return false;
  }
}

// Metadata helpers

static inline uint64_t thread_metadata_pages () {
  // Round up at page granularity
  return ( sizeof(thread_metadata_t)+(PAGE_SIZE-1) ) / PAGE_SIZE;
}

static inline uint64_t enclave_metadata_pages ( uint64_t num_mailboxes ) {
  // Round up at page granularity
  size_t enclave_size = sizeof(enclave_metadata_t) + num_mailboxes*sizeof(mailbox_t);
  return ( enclave_size+(PAGE_SIZE-1) ) / PAGE_SIZE;
}

api_result_t add_lock_region_iff_free_metadata_pages (uintptr_t ptr, uint64_t num_pages, region_map_t * locked_regions);

api_result_t add_lock_region_iff_valid_metadata(uintptr_t ptr, metadata_page_t metadata_type, region_map_t * locked_regions);

static inline api_result_t add_lock_region_iff_valid_enclave (uintptr_t ptr, region_map_t * locked_regions) {
  return add_lock_region_iff_valid_metadata( ptr, METADATA_PAGE_ENCLAVE, locked_regions);
}

static inline api_result_t add_lock_region_iff_valid_thread (uintptr_t ptr, region_map_t * locked_regions) {
  return add_lock_region_iff_valid_metadata( ptr, METADATA_PAGE_THREAD, locked_regions);
}

// Page Table helper

api_result_t load_page_table_entry (enclave_id_t enclave_id,
  uintptr_t phys_addr,
  uintptr_t virtual_addr,
  uint64_t level,
  uintptr_t acl,
  region_map_t *locked_regions);

// LLC partitioning helper

int handle_llc_changes(void);

// LLC flushing helper

void flush_llc_region(int region_id);

#endif // SECURITY_MONITOR_H
