#ifndef ECALL_S_H
#define ECALL_S_H
#include <data_structures.h>

// ENCLAVE MANAGMENT

api_result_t ecall_exit_enclave(uintptr_t *regs);

// THREAD MANAGMENT

api_result_t ecall_accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr);

// MAILBOXES

api_result_t ecall_get_attestation_key(uintptr_t phys_addr);
api_result_t ecall_accept_message(mailbox_id_t mailbox_id, enclave_id_t expected_sender);
api_result_t ecall_read_message(mailbox_id_t mailbox_id, uintptr_t phys_addr);
api_result_t ecall_send_message(enclave_id_t enclave_id, mailbox_id_t mailbox_id, uintptr_t phys_addr);

// DRAM REGION

api_result_t ecall_enclave_block_dram_region(dram_region_id_t id);
api_result_t ecall_dram_region_check_ownership(dram_region_id_t id);

#endif // ECALL_S_H
