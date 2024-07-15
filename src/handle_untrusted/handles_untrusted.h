#ifndef SM_HANDLES_H
#define SM_HANDLES_H

#include "api_types.h"

// Handles for SM API
api_result_t sm_internal_enclave_create (enclave_id_t enclave_id, uintptr_t ev_base, uintptr_t ev_mask, uint64_t num_mailboxes, bool debug);

api_result_t sm_internal_enclave_delete (enclave_id_t enclave_id);

api_result_t sm_internal_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs);

api_result_t sm_internal_enclave_init (enclave_id_t enclave_id);

api_result_t sm_internal_enclave_load_handler (enclave_id_t enclave_id, uintptr_t phys_addr);

api_result_t sm_internal_enclave_load_page_table (enclave_id_t enclave_id, phys_ptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl);

api_result_t sm_internal_enclave_load_page (enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl);

uint64_t sm_internal_enclave_metadata_pages (uint64_t num_mailboxes);

api_result_t sm_internal_enclave_get_attest (enclave_id_t enclave_id, uintptr_t addr_m, uintptr_t addr_pk, uintptr_t addr_s);

api_result_t sm_internal_get_public_field (public_field_t field, uintptr_t addr);

api_result_t sm_internal_mail_accept (mailbox_id_t mailbox_id, enclave_id_t expected_sender);

api_result_t sm_internal_mail_receive (mailbox_id_t mailbox_id, phys_ptr_t out_message, phys_ptr_t out_sender_measurement);

api_result_t sm_internal_mail_send (enclave_id_t enclave_id, mailbox_id_t mailbox_id, phys_ptr_t phys_addr);

api_result_t sm_internal_region_assign (region_id_t id, enclave_id_t new_owner);

api_result_t sm_internal_region_block (region_id_t id);

api_result_t sm_internal_region_update ();

api_result_t sm_internal_region_free (region_id_t id);

api_result_t sm_internal_region_metadata_create (region_id_t dram_region);

uint64_t sm_internal_region_metadata_pages();

uint64_t sm_internal_region_metadata_start();

enclave_id_t sm_internal_region_owner (region_id_t id);

region_state_t sm_internal_region_state (region_id_t id);

api_result_t sm_internal_region_cache_partitioning (cache_partition_t *part);

api_result_t sm_internal_region_flush (region_id_t id);

api_result_t sm_internal_thread_delete (thread_id_t thread_id);

api_result_t sm_internal_thread_load (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t entry_pc, uintptr_t entry_stack, uint64_t timer_limit);

uint64_t sm_internal_thread_metadata_pages();


#endif // SM_HANDLES_H
