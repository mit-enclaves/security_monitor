#ifndef SECURITY_MONITOR_H
#define SECURITY_MONITOR_H

#include "sm_types.h"
#include "sm_constants.h"
#include "api.h"
#include <platform.h>

// Helpful macros
// --------------

#define get_abs_addr(symbol) ({  \
  void * __tmp; \
  asm volatile ( \
    "lui %0, %%hi(" #symbol ") \n" \
    "addi %0, %0, %%lo(" #symbol ") \n" \
    : "=r"(__tmp)); \
  __tmp; \
})

/*
#define get_abs_addr(reg) ({ void * __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })
*/

// Common minor operations
// -----------------------

// Global state accessors
static inline sm_state_t * get_sm_state_ptr (void) {
  return get_abs_addr(sm_state);
}

static inline sm_keys_t * get_sm_keys_ptr (void) {
  return get_abs_addr(sm_keys);
}


// Region helpers
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

static inline bool addr_to_region_page_id (uintptr_t addr) {
  return ((addr & REGION_MASK) >> PAGE_SHIFT);
}

static inline bool is_valid_page_id_in_region (uint64_t page_id) {
  return page_id < NUM_REGION_PAGES;
}


// Synchronization helpers
static inline bool lock_untrusted_state () {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_acquire(&sm->untrusted_state_lock);
}

static inline void unlock_untrusted_state () {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_release(&sm->untrusted_state_lock);
}

static inline bool lock_region (region_id_t region_id) {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_acquire(&sm->regions[region_id].lock);
}

static inline void unlock_region (region_id_t region_id) {
  sm_state_t * sm = get_sm_state_ptr();
  platform_lock_release(&sm->regions[region_id].lock);
}

static inline void unlock_regions (region_map_t * locked_regions) {
  sm_state_t * sm = get_sm_state_ptr();
  for (int i=0; i<NUM_REGIONS; i++) {
    if (locked_regions->flags[i]) {
      unlock_region(i);
    }
  }
}


// Metadata helpers
api_result_t lock_region_iff_free_metadata_pages (uintptr_t ptr, uint64_t num_pages);

api_result_t lock_region_iff_valid_metadata( uintptr_t ptr, metadata_page_t metadata_type );

static inline bool lock_region_iff_valid_enclave (uintptr_t ptr) {
  return lock_region_iff_valid_metadata( ptr, METADATA_PAGE_ENCLAVE);
}

static inline bool lock_region_iff_valid_thread (uintptr_t ptr) {
  return lock_region_iff_valid_metadata( ptr, METADATA_PAGE_THREAD);
}

#endif // SECURITY_MONITOR_H
