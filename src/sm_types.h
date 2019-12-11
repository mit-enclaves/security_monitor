#ifndef SECURITY_MONITOR_TYPES_H
#define SECURITY_MONITOR_TYPES_H

#include "sm_constants.h"
#include "api.h"
#include <platform_types.h>
#include <cryptography.h>
#include <secure_boot/secure_boot.h>
#include <stdint.h>
#include <stdbool.h>

// SM Types
// --------

typedef struct region_map_t {
  bool flags[NUM_REGIONS];
} region_map_t;

typedef enum {
  ENCLAVE_MAILBOX_STATE_UNUSED = 0,
  ENCLAVE_MAILBOX_STATE_EMPTY = 1,
  ENCLAVE_MAILBOX_STATE_FULL = 2,
} enclave_mailbox_state_t;

typedef struct mailbox_t {
  enclave_mailbox_state_t state;
  enclave_id_t expected_sender;
  hash_t sender_measurement;
  uint8_t message[MAILBOX_SIZE];
} mailbox_t;

typedef enum {
  ENCLAVE_STATE_CREATED = 0,
  ENCLAVE_STATE_HANDLER_LOADED = 1,
  ENCLAVE_STATE_PAGE_TABLES_LOADED = 2,
  ENCLAVE_STATE_PAGE_DATA_LOADED = 3,
  ENCLAVE_STATE_INITIALIZED = 4,
} enclave_init_state_t;

typedef struct enclave_metadata_t {
  // Initialization state
  enclave_init_state_t init_state;
  uintptr_t last_phys_addr_loaded;
  hash_context_t hash_context;

  // Parameters
  uintptr_t ev_base;
  uintptr_t ev_mask;
  int64_t num_mailboxes;
  bool debug;

  // Measurement
  hash_t measurement;

  // State
  int64_t num_threads;
  region_map_t regions;
  mailbox_t mailboxes[];
} enclave_metadata_t;

typedef struct thread_metadata_t {
  // Parameters
  uintptr_t entry_pc;
  uintptr_t entry_sp;

  // State
  platform_lock_t is_scheduled;

  // Untrusted core state at enclave_enter
  uintptr_t untrusted_pc;
  uintptr_t untrusted_sp;
  platform_core_state_t untrusted_state;

  // Enclave state buffer in the event of a trap/interrupt/fault
  platform_core_state_t fault_state;

  // AEX - asynchronous enclave exit state
  bool aex_present;
  platform_core_state_t aex_state;
} thread_metadata_t;

typedef uint8_t page_t[PAGE_SIZE];

typedef uint8_t page_map_t[NUM_REGION_PAGES];

typedef union metadata_region_t {
  page_map_t page_info;
  page_t pages[NUM_REGION_PAGES];
} metadata_region_t;

#define get_metadata_start_page() ( 1 + ( (sizeof(page_map_t)+PAGE_SIZE-1) / PAGE_SIZE ) )

typedef struct sm_core_t {
  enclave_id_t owner;
  thread_id_t thread;

  platform_lock_t lock;
} sm_core_t;

typedef struct sm_region_t {
  enclave_id_t owner;

  region_type_t type;
  region_state_t state;

  platform_lock_t lock;
} sm_region_t;

typedef struct sm_state_t {
  sm_core_t cores[NUM_CORES];
  sm_region_t regions[NUM_REGIONS];
  hash_t signing_enclave_measurement;
  region_map_t untrusted_regions;
  mailbox_t untrusted_mailboxes[NUM_UNTRUSTED_MAILBOXES];
  platform_lock_t untrusted_state_lock;
} sm_state_t;

typedef boot_image_header_t sm_keys_t;

#endif // SECURITY_MONITOR_H
