#ifndef SECURITY_MONITOR_H
#define SECURITY_MONITOR_H
#include <constants.h>
#include <platform.h>

// Helpful macros
// --------------

#define get_abs_addr(symbol) ({  \
  unsigned long __tmp;    \
  asm volatile (          \
    "lui %0, %hi("STR(symbol)")" \
    "addi %0, %lo("STR(symbol)")" \
    : "=r"(__tmp));       \
  __tmp;                  \
})

define assert(expr, message) \
  if ( !(expr) ) \
    #error "assert error : " #message

// Validate parameterization
// -------------------------

// TODO: ENUMs fit in their respective bit fields

// TODO: region_type_t fits into uint8_t

assert(NUM_CORES == 1, "One core is currently supported - see TODOs")
assert(NUM_REGIONS <= 64, "Up to XLEN=64 regions are supported here")

// SM Types
// --------

typedef uint64_t phys_ptr_t;

typedef enclave_t * enclave_id_t;
typedef thread_t * thread_id_t;

typedef enum {
  ENCLAVE_MAILBOX_STATE_UNUSED = 0,
  ENCLAVE_MAILBOX_STATE_EMPTY = 1,
  ENCLAVE_MAILBOX_STATE_FULL = 2,
} enclave_mailbox_state_t;

typedef struct{
  enclave_mailbox_state_t state;
  enclave_id_t expected_sender;
  hash_t sender_measurement
  uint8_t message[MAILBOX_SIZE];
} mailbox_t;

typedef enum {
  ENCLAVE_STATE_CREATED = 0,
  ENCLAVE_STATE_HANDLER_LOADED = 1,
  ENCLAVE_STATE_PAGE_TABLES_LOADED = 2,
  ENCLAVE_STATE_PAGE_DATA_LOADED = 3,
  ENCLAVE_STATE_INITIALIZED = 4,
} enclave_init_state_t;

typedef struct {
  // Initialization state
  enclave_init_state_t init_state;
  uintptr_t last_phys_addr_loaded;
  hash_context_t hash_context;

  // Parameters
  uintptr_t evbase;
  uintptr_t evmask;
  int64_t num_mailboxes;
  bool debug;

  // Measurement
  hash_t measurement;

  // State
  int64_t num_threads;
  region_map_t regions;
  mailbox_t mailboxes[];
} enclave_t;

typedef struct {
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
} thread_t;

typedef uint8_t page_t[PAGE_SIZE];

typedef union {
  uint8_t page_info[NUM_REGION_PAGES];
  page_t pages[NUM_REGION_PAGES];
} metadata_region_t;

typedef struct {
  enclave_id_t owner;
  thread_id_t thread;

  platform_lock_t lock;
} core_t;

typedef struct {
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
  platform_lock_t untrusted_regions_lock;
} sm_state_t;

// Common minor operations
// -----------------------

static inline bool is_page_aligned (uintptr_t addr) {
  return (enclave_id % PAGE_SIZE);
}

static inline uint64_t addr_to_region_id (uintptr_t addr) {
  return ((addr-RAM_BASE) & REGION_MASK) >> REGION_SHIFT; // will return an illegally large number in case of an address outside RAM. CAUTION!
}

static inline uint8_t * region_id_to_addr (uint64_t region_id) {
  return RAM_BASE + (region_id << REGION_SHIFT);
}

static inline bool is_valid_region_id (uint64_t region_id) {
  return (region_id < NUM_REGIONS);
}

static inline bool addr_to_region_page_id (uintptr_t addr) {
  return ((addr & REGION_MASK) >> PAGE_SHIFT);
}

static inline bool is_valid_page_id_in_region (uint64_t page_id) {
  return page_id < NUM_REGION_PAGES;
}

static inline sm_state_t * get_sm_state_ptr (void) {
  return get_abs_addr(sm_state);
}

static inline sm_keys_t * get_sm_keys_ptr (void) {
  return get_abs_addr(sm_keys);
}

#endif // SECURITY_MONITOR_H
