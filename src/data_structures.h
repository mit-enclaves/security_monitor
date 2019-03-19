#ifndef SECURITY_MONITOR_DATA_STRUCTURES_H
#define SECURITY_MONITOR_DATA_STRUCTURES_H

#include <stdint.h>
#include "constants.h"

typedef uint64_t phys_ptr_t;
typedef uint64_t hash_t[8];

typedef struct {
	uint64_t registers[32];
}core_states_t;

// ATOMIC FLAG
typedef struct {
	uint64_t flag;
	uint64_t pad[7];
}atomic_flag_t;

// MAILBOX
typedef phys_ptr_t enclave_id_t;

typedef struct{
	enclave_id_t expected_sender;
	bool has_message;
	uint8_t message[MAILBOX_SIZE];
}mailbox_t;

// ENCLAVE


typedef struct {
	bool initialized;
	bool debug;
	int64_t thread_cout;
	int64_t dram_bitmap;
	hash_t measurement;
	int64_t mailbox_count;
	mailbox_t first_mailbox;
}enclave_t;

// THREAD

typedef struct {
	atomic_flag_t is_scheduled;
	bool aes_present;
	void *untrusted_pc;
	void *untrusted_sp;
	void *page_table_ptr;
	void *entry_pc;
	void *entry_sp;
	void *fault_pc;
	void *fault_sp;
	core_states_t fault_state;
	core_states_t aex_state;
}thread_t;

// METADATE PAGE MAP

typedef uint64_t metadata_page_map_entry;

typedef metadata_page_map_entry metadata_page_map_t[NUM_PAGES_PER_REGION];

typedef enum { // TODO must fit into 12 bits
	invalid = 0,
	free    = 1,
	enclave = 2,
	thread  = 3,
}metadata_page_t;

// CORE

typedef struct {
	enclave_t owner;
	bool has_enclave_schedule;
	thread_t cur_thread;
	atomic_flag_t lock;
}core_t;

// DRAM REGION

typedef enum {
	dram_region_invalid = 0,
	dram_region_free = 1,
	dram_region_blocked = 2,
	dram_region_locked = 3,
	dram_region_owned = 4,
} dram_region_state_t;

typedef enum {
	untrusted_region = 0,
	enclave_region = 1,
	metadata_region = 2,
	security_monitor_region = 3,
}dram_region_type_t;

typedef struct {
	enclave_t owner;
	dram_region_type_t type;
	dram_region_state_t state;
	atomic_flag_t lock;
}region_t;

// SECURITY MONITOR

typedef struct {
	core_t cores[NUM_CORES];
	region_t regions[NUM_REGIONS];

}security_monitor_t;

#endif // SECURITY_MONITOR_DATA_STRUCTURES_H
