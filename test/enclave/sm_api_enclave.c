#include <api.h>
#include <enclave/enclave_util.h>

// TODO: fix this
//api_result_t block_dram_region(dram_region_id_t id) {
//   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_BLOCK_DRAM_REGION, id, 0, 0);
//}

E_CODE api_result_t dram_region_check_ownership(dram_region_id_t id) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_CHECK_OWNERSHIP, id, 0, 0);
}

E_CODE api_result_t get_attestation_key(uintptr_t phys_addr) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_GET_ATTESTATION_KEY, phys_addr, 0, 0);
}

E_CODE api_result_t accept_message(mailbox_id_t mailbox_id, enclave_id_t expected_sender) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_ACCEPT_MESSAGE, mailbox_id, expected_sender, 0);
}

E_CODE api_result_t read_message(mailbox_id_t mailbox_id, uintptr_t phys_addr) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_READ_MESSAGE, mailbox_id, phys_addr, 0);
}

E_CODE api_result_t send_message(enclave_id_t enclave_id, mailbox_id_t mailbox_id,
      uintptr_t phys_addr) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_SEND_MESSAGE, enclave_id, mailbox_id, phys_addr);
}

E_CODE api_result_t accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr) {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_ACCEPT_THREAD, thread_id, thread_info_addr, 0);
}

E_CODE api_result_t exit_enclave() {
   return UBI_SM_ENCLAVE_CALL(UBI_SM_ENCLAVE_EXIT_ENCLAVE, 0, 0, 0);
}
