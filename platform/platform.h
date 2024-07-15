#ifndef SM_PLATFORM_H
#define SM_PLATFORM_H

#include "platform_types.h"
#include "platform_assert.h"
#include <parameters.h>
#include <csr/csr_util.h>
#include <sm_types.h>
#include <platform_lock.h>

// Atomic operations

#define mb() asm volatile ("fence" ::: "memory")
#define atomic_set(ptr, val) (*(volatile typeof(*(ptr)) *)(ptr) = val)
#define atomic_read(ptr) (*(volatile typeof(*(ptr)) *)(ptr))

# define atomic_add(ptr, inc) __sync_fetch_and_add(ptr, inc)
# define atomic_or(ptr, inc) __sync_fetch_and_or(ptr, inc)
# define atomic_swap(ptr, swp) __sync_lock_test_and_set(ptr, swp)
# define atomic_cas(ptr, cmp, swp) __sync_val_compare_and_swap(ptr, cmp, swp)

// mtime device
extern volatile uint64_t* mtime;

// Core state
// ----------
//void platform_save_core( platform_core_state_t * core_state );
//void platform_load_core( const platform_core_state_t * core_state );
void platform_clean_core(void); // arch state only
void platform_purge_core (void); // uarch state only

// Platform control and utilities
// ------------------------------
#define platform_get_core_id() read_csr(mhartid)

#define clean_reg(reg) ({ \
  asm volatile ("li " #reg ", 0"); })

#define write_reg(reg, val) ({ \
  asm volatile ("ld " #reg ", %0" :: "rK"(val)); })

void platform_init (void);
void platform_core_init (void);

#define platform_get_device_tree_addr() ((uintptr_t) (((uint64_t) UNTRUSTED_ENTRY) + ((uint64_t) PAYLOAD_MAXLEN)))

void platform_interrupt_other_cores (void);
void platform_wait_for_interrupt (void);

void platform_delegate_to_untrusted ( uint64_t virtual_pc, uint64_t  ) __attribute__((noreturn));
void platform_jump_to_untrusted ( uint64_t virtual_pc, uint64_t virtual_sp, uint64_t core_id, uintptr_t dt_addr) __attribute__((noreturn));

void platform_initialize_memory_protection(sm_state_t *sm, int core_id);

void platform_set_enclave_page_table(enclave_metadata_t *enclave_metadata, thread_metadata_t *thread_metadata);
void platform_restore_untrusted_page_table(thread_metadata_t *thread_metadata);

void platform_protect_enclave_sm_handler(enclave_metadata_t *enclave_metadata, uintptr_t phys_addr);

void platform_update_untrusted_regions(sm_state_t* sm, int core_id, uint64_t index_id, bool flag);
void platform_update_enclave_regions(sm_state_t* sm, int core_id, enclave_metadata_t *enclave_metadata, uint64_t index_id, bool flag);

void platform_update_memory_protection();

void platform_memory_protection_enter_enclave(sm_core_t *core, enclave_metadata_t *enclave_metadata, thread_metadata_t *thread_metadata);
void platform_memory_protection_exit_enclave(sm_core_t *core, thread_metadata_t *thread_metadata);

void platform_interrupts_enter_enclave(thread_metadata_t *thread_metadata);
void platform_interrupts_exit_enclave(thread_metadata_t *thread_metadata);

void platform_panic(void) __attribute__((noreturn));

void platform_disable_speculation();
void platform_enable_speculation();
void platform_disable_predictors();
void platform_enable_predictors();
void platform_disable_L1();
void platform_enable_L1();

// Platform helpers

uint64_t regions_to_bitmap(region_map_t *regions);
region_map_t bitmap_to_regions(uint64_t bitmap);

static inline bool region_is_accessible(uint64_t mrbm, uint64_t region_id) {
  uint64_t mask = 1ul << region_id;
  return ((mrbm & mask) != 0);
}

#endif // SM_PLATFORM_H
