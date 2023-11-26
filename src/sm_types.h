#ifndef SECURITY_MONITOR_TYPES_H
#define SECURITY_MONITOR_TYPES_H

#include "sm_constants.h"
#include "crypto/cryptography.h"
#include <api_types.h>
#include <platform_types.h>
#include <stdint.h>
#include <stdbool.h>

// SM Types
// --------

typedef struct sm_keys_t {
  public_key_t manufacturer_public_key;

  uint32_t device_public_key_present;
  public_key_t device_public_key;
  signature_t device_signature;

  uint32_t software_public_key_present;
  hash_t software_measurement;
  public_key_t software_public_key;
  secret_key_t software_secret_key;
  signature_t software_signature;

  size_t software_measured_bytes;
  uint8_t* software_measured_binary[];
} sm_keys_t;

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
  enclave_platform_csr_t platform_csr;

  int64_t num_mailboxes;
  bool debug;

  // Measurement, Keys and Attestation
  hash_t measurement;
  public_key_t public_key;
  secret_key_t secret_key;
  signature_t attestation;
 
  // SM pc and sp base
  uintptr_t fault_pc;
  uintptr_t fault_sp_base;

  // State
  int64_t num_threads;
  region_map_t regions;
  mailbox_t mailboxes[];
} enclave_metadata_t;

typedef struct thread_metadata_t {
  // Owner
  enclave_id_t owner;

  // Parameters
  uintptr_t entry_pc;
  uintptr_t entry_sp;

  // State
  bool is_scheduled;

  // Untrusted core state at enclave_enter
  uintptr_t untrusted_pc;
  platform_core_state_t untrusted_state;

  // Untrusted fault sp and pc
  uintptr_t untrusted_fault_pc;
  uintptr_t untrusted_fault_sp;

  // Enclave state buffer in the event of a trap/interrupt/fault
  platform_core_state_t fault_state;

  // AEX - asynchronous enclave exit state
  bool aex_present;
  platform_core_state_t aex_state;
  
  // Thread timer limit
  int64_t timer_limit;

  // Save Platform Specific Context
  thread_platform_csr_t platform_csr;
} thread_metadata_t;

typedef enum { // NOTE must fit into 12 bits
  METADATA_PAGE_INVALID = 0,
  METADATA_PAGE_FREE = 1,
  METADATA_PAGE_ENCLAVE = 2,
  METADATA_PAGE_THREAD = 3,
} metadata_page_t;

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
  uintptr_t hls_ptr;
  
  // Memory regions bit maps
  uint64_t mmrbm;
  uint64_t memrbm;

  platform_lock_t lock;
} sm_core_t;

typedef struct sm_region_t {
  enclave_id_t owner;

  region_type_t type;
  region_state_t state;

  platform_lock_t lock;
} sm_region_t;

typedef struct llc_sync_t {
  volatile uint64_t waiting;
  volatile bool wait;
  volatile uint64_t left;
  volatile bool busy;
  platform_lock_t lock;
} llc_sync_t;

#define BOOT_INIT_NOT_DONE (12345)
#define BOOT_INIT_DONE     (98765)

typedef struct sm_state_t {
  sm_keys_t keys;
  sm_core_t cores[NUM_CORES];
  sm_region_t regions[NUM_REGIONS];
  region_map_t untrusted_regions;
  mailbox_t untrusted_mailboxes[NUM_UNTRUSTED_MAILBOXES];
  cache_partition_t llc_partitions;
  llc_sync_t llc_sync;
  platform_lock_t console_lock;
  platform_lock_t untrusted_state_lock;
} sm_state_t;

#endif // SECURITY_MONITOR_H
