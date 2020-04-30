#ifndef API_UNSTRUSTED_H
#define API_UNSTRUSTED_H

#include "api_types.h"
#include "api_util.h"

static inline api_result_t sm_enclave_create (enclave_id_t enclave_id, uintptr_t ev_base, uintptr_t ev_mask, uint64_t num_mailboxes, uint64_t timer_limit, bool debug) {
  return SM_API_CALL(SM_ENCLAVE_CREATE, enclave_id, ev_base, ev_mask, num_mailboxes, timer_limit, debug);
}

static inline api_result_t sm_enclave_delete (enclave_id_t enclave_id) {
  return SM_API_CALL(SM_ENCLAVE_DELETE, enclave_id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id) {
  return SM_API_CALL(SM_ENCLAVE_ENTER, enclave_id, thread_id, 0, 0, 0, 0);
}

static inline api_result_t sm_enclave_init (enclave_id_t enclave_id) {
  return SM_API_CALL(SM_ENCLAVE_INIT, enclave_id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_enclave_load_handler (enclave_id_t enclave_id, uintptr_t phys_addr) {
  return SM_API_CALL(SM_ENCLAVE_LOAD_HANDLER, enclave_id, phys_addr, 0, 0, 0, 0);
}

static inline api_result_t sm_enclave_load_page_table (enclave_id_t enclave_id, phys_ptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {
  return SM_API_CALL(SM_ENCLAVE_LOAD_PAGE_TABLE, enclave_id, phys_addr, virtual_addr, level, acl, 0);
}

static inline api_result_t sm_enclave_load_page (enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl) {
  return SM_API_CALL(SM_ENCLAVE_LOAD_PAGE, enclave_id, phys_addr, virtual_addr, os_addr, acl, 0);
}

static inline uint64_t sm_enclave_metadata_pages (uint64_t num_mailboxes) {
  return SM_API_CALL(SM_ENCLAVE_METADATA_PAGES, num_mailboxes, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_get_public_field (public_field_t field, phys_ptr_t phys_addr) {
  return SM_API_CALL(SM_GET_PUBLIC_FIELD, field, phys_addr, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_accept (mailbox_id_t mailbox_id, enclave_id_t expected_sender) {
  return SM_API_CALL(SM_MAIL_ACCEPT, mailbox_id, expected_sender, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_receive (mailbox_id_t mailbox_id, phys_ptr_t out_message, phys_ptr_t out_sender_measurement) {
  return SM_API_CALL(SM_MAIL_RECEIVE, mailbox_id, out_message, out_sender_measurement, 0, 0, 0);
}

static inline api_result_t sm_mail_send (enclave_id_t enclave_id, mailbox_id_t mailbox_id, phys_ptr_t phys_addr) {
  return SM_API_CALL(SM_MAIL_SEND, enclave_id, mailbox_id, phys_addr, 0, 0, 0);
}

static inline api_result_t sm_region_assign (region_id_t id, enclave_id_t new_owner) {
  return SM_API_CALL(SM_REGION_ASSIGN, id, new_owner, 0, 0, 0, 0);
}

static inline api_result_t sm_region_block (region_id_t id) {
  return SM_API_CALL(SM_REGION_BLOCK, id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_flush () {
  return SM_API_CALL(SM_REGION_FLUSH, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_free (region_id_t id) {
  return SM_API_CALL(SM_REGION_FREE, id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_metadata_create (region_id_t dram_region) {
  return SM_API_CALL(SM_REGION_METADATA_CREATE, dram_region, 0, 0, 0, 0, 0);
}

static inline uint64_t sm_region_metadata_pages() {
  return SM_API_CALL(SM_REGION_METADATA_PAGES, 0, 0, 0, 0, 0, 0);
}

static inline uint64_t sm_region_metadata_start() {
  return SM_API_CALL(SM_REGION_METADATA_START, 0, 0, 0, 0, 0, 0);
}

static inline enclave_id_t sm_region_owner (region_id_t id) {
  return SM_API_CALL(SM_REGION_OWNER, id, 0, 0, 0, 0, 0);
}

static inline region_state_t sm_region_state (region_id_t id) {
  return SM_API_CALL(SM_REGION_STATE, id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_thread_delete (thread_id_t thread_id) {
  return SM_API_CALL(SM_THREAD_DELETE, thread_id, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_thread_load (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc, uintptr_t fault_stack) {
  return SM_API_CALL(SM_THREAD_LOAD, enclave_id, thread_id, entry_pc, entry_stack, fault_pc, fault_stack);
}

static inline uint64_t sm_thread_metadata_pages() {
  return SM_API_CALL(SM_THREAD_METADATA_PAGES, 0, 0, 0, 0, 0, 0);
}

#endif // API_UNSTRUSTED_H
