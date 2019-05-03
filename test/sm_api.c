#include <api.h>
#include <sm_util/sm_util.h>

__attribute__((section(".os.text")))

api_result_t block_dram_region(dram_region_id_t id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_BLOCK_DRAM_REGION, id, 0, 0, 0, 0, 0);
}

api_result_t dram_region_check_ownership(dram_region_id_t id) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_CHECK_OWNERSHIP, id, 0, 0, 0, 0, 0);
}

api_result_t get_attestation_key(uintptr_t phys_addr) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_GET_ATTESTATION_KEY, phys_addr, 0, 0, 0, 0, 0);
}

api_result_t accept_message(mailbox_id_t mailbox_id, enclave_id_t expected_sender) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_ACCEPT_MESSAGE, mailbox_id, expected_sender, 0, 0, 0, 0);
}

api_result_t read_message(mailbox_id_t mailbox_id, uintptr_t phys_addr) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_READ_MESSAGE, mailbox_id, phys_addr, 0, 0, 0, 0);
}

api_result_t send_message(enclave_id_t enclave_id, mailbox_id_t mailbox_id,
      uintptr_t phys_addr) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_SEND_MESSAGE, enclave_id, mailbox_id, 0, 0, 0, 0);
}

api_result_t accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr) {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_ACCEPT_THREAD, thread_id, thread_info_addr, 0, 0, 0, 0);
}

api_result_t exit_enclave() {
   return SBI_SM_OS_CALL(UBI_SM_ENCLAVE_EXIT_ENCLAVE, 0, 0, 0, 0, 0, 0);
}

dram_region_state_t dram_region_state(dram_region_id_t id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_DRAM_REGION_STATE, id, 0, 0, 0, 0, 0);
}

enclave_id_t dram_region_owner(dram_region_id_t id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_DRAM_REGION_OWNER, id, 0, 0, 0, 0, 0);
}

api_result_t assign_dram_region(dram_region_id_t id, enclave_id_t new_owner) {
   return SBI_SM_OS_CALL(SBI_SM_OS_ASSIGN_DRAM_REGION, id, new_owner, 0, 0, 0, 0);
}

api_result_t free_dram_region(dram_region_id_t id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_FREE_DRAM_REGION, id, 0, 0, 0, 0, 0);
}

api_result_t flush_cached_dram_regions() {
   return SBI_SM_OS_CALL(SBI_SM_OS_FLUSH_CACHED_DRAM_REGIONS, 0, 0, 0, 0, 0, 0);
}

api_result_t create_metadata_region(dram_region_id_t dram_region) {
   return SBI_SM_OS_CALL(SBI_SM_OS_CREATE_METADATA_REGION, dram_region, 0, 0, 0, 0, 0);
}

uint64_t metadata_region_pages() {
   return SBI_SM_OS_CALL(SBI_SM_OS_METADATA_REGION_PAGES, 0, 0, 0, 0, 0, 0);
}

uint64_t metadata_region_start() {
   return SBI_SM_OS_CALL(SBI_SM_OS_METADATA_REGION_START, 0, 0, 0, 0, 0, 0);
}

uint64_t thread_metadata_pages() {
   return SBI_SM_OS_CALL(SBI_SM_OS_THREAD_METADATA_PAGES, 0, 0, 0, 0, 0, 0);
}

uint64_t enclave_metadata_pages(uint64_t mailbox_count) {
   return SBI_SM_OS_CALL(SBI_SM_OS_ENCLAVE_METADATA_PAGES, mailbox_count, 0, 0, 0, 0, 0);
}

api_result_t create_enclave(enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug) {
   return SBI_SM_OS_CALL(SBI_SM_OS_CREATE_ENCLAVE, enclave_id, ev_base, ev_mask, mailbox_count, debug, 0);
}

api_result_t load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {
   return SBI_SM_OS_CALL(SBI_SM_OS_LOAD_PAGE_TABLE, enclave_id, phys_addr, virtual_addr, level, acl, 0);
}

api_result_t load_page(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl) {
   return SBI_SM_OS_CALL(SBI_SM_OS_LOAD_PAGE, enclave_id, phys_addr, virtual_addr, os_addr, acl, 0);
}

api_result_t init_enclave(enclave_id_t enclave_id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_INIT_ENCLAVE, enclave_id, 0, 0, 0, 0, 0);
}

api_result_t delete_enclave(enclave_id_t enclave_id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_DELETE_ENCLAVE, enclave_id, 0, 0, 0, 0, 0);
}

api_result_t enter_enclave(enclave_id_t enclave_id, thread_id_t thread_id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_ENTER_ENCLAVE, enclave_id, thread_id, 0, 0, 0, 0);
}

api_result_t load_thread(enclave_id_t enclave_id, thread_id_t thread_id,
    uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc,
    uintptr_t fault_stack) {
   return SBI_SM_OS_CALL(SBI_SM_OS_LOAD_THREAD, enclave_id, thread_id, entry_pc, entry_stack, fault_pc, fault_stack);
}

api_result_t assign_thread(enclave_id_t enclave_id, thread_id_t thread_id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_ASSIGN_THREAD, enclave_id, thread_id, 0, 0, 0, 0);
}

api_result_t delete_thread(thread_id_t thread_id) {
   return SBI_SM_OS_CALL(SBI_SM_OS_DELETE_THREAD, thread_id, 0, 0, 0, 0, 0);
}
