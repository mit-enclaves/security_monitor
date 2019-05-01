#ifndef ECALL_S_H
#define ECALL_S_H
#include <data_structures.h>

// METADATA

api_result_t ecall_create_metadata_region(dram_region_id_t id);
uint64_t ecall_metadata_region_pages();
uint64_t ecall_metadata_region_start();
uint64_t ecall_thread_metadata_pages();
uint64_t ecall_enclave_metadata_pages(uint64_t mailbox_count);

//ENCLAVE MANAGMENT

api_result_t ecall_create_enclave(enclave_id_t enclave_id, uintptr_t ev_base, uintptr_t ev_mask, uint64_t mailbox_count, bool debug);
api_result_t ecall_load_trap_handler(enclave_id_t enclave_id, uintptr_t phys_addr);
api_result_t ecall_load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uint64_t level, uintptr_t acl);
api_result_t ecall_load_page(enclave_id_t enclave_id, uintptr_t phys_addr, uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl);
api_result_t ecall_init_enclave(enclave_id_t enclave_id);
api_result_t ecall_delete_enclave(enclave_id_t enclave_id);
api_result_t ecall_enter_enclave(enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs);

// THREAD MANAGMENT

api_result_t ecall_allocate_thread(enclave_id_t enclave_id, thread_id_t thread_id);
api_result_t ecall_load_thread(enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc, uintptr_t fault_stack);
api_result_t ecall_assign_thread(enclave_id_t enclave_id, thread_id_t thread_id);
api_result_t ecall_delete_thread(thread_id_t thread_id);

// DRAM REGION

dram_region_state_t ecall_dram_region_state(dram_region_id_t id);
enclave_id_t ecall_dram_region_owner(dram_region_id_t id);
api_result_t ecall_assign_dram_region(dram_region_id_t id, enclave_id_t new_owner);
api_result_t ecall_os_block_dram_region(dram_region_id_t id);
api_result_t ecall_free_dram_region(dram_region_id_t id);

#endif // ECALL_S_H
