#include "../htif/htif.h"
#include <api.h>

#include <sm_util/sm_util.h>

#define SIZE_REGION (0x2000000)

#define leaf_permissions (0b11101111) // D A G (not U) X W R V
#define node_permissions (0b00000001) // Node

__attribute__((section(".os.text"))) int main(void) {

   api_result_t res = monitor_ok;
   uint64_t ret = 0;

   dram_region_id_t mr_id = 4;
   
   print_str("create_metadata_region(3) : ");
   res = create_metadata_region(mr_id);
   print_api_r(res);
   print_str("\n");

   print_str("metadata_region_start() : ");
   ret = metadata_region_start();
   print_int(ret);
   print_str("\n");

   enclave_id_t enclave_id = DRAM_START + (mr_id * SIZE_REGION) + (ret * SIZE_PAGE);

   print_str("enclave_id :");
   print_int(enclave_id);
   print_str("\n");

   uint64_t evmask = ~((0x2000000) - 1);
   print_str("create_enclave(enclave_id, 0, evmask, 1, false) : ");
   res = create_enclave(enclave_id, 0, evmask, 1, false);
   print_api_r(res);
   print_str("\n");

   dram_region_id_t er_id = 5;
   
   print_str("assign_dram_region(er_id, enclave_id)");
   res = assign_dram_region(er_id, enclave_id);
   print_api_r(res);
   print_str("\n");

   uintptr_t phys_addr = DRAM_START + (er_id * SIZE_REGION);
  
   uintptr_t trap_handler_entry = phys_addr;

   print_str("load_trap_handler(enclave_id, phys_addr)");
   res = load_trap_handler(enclave_id, phys_addr);
   print_api_r(res);
   print_str("\n");

   phys_addr += SIZE_PAGE;
   //STACK
   phys_addr += SIZE_PAGE;
   uintptr_t fault_stack_ptr = phys_addr;

   print_str("load_page_table(enclave_id, phys_addr, 0, 3, node_permissions)");
   res = load_page_table(enclave_id, phys_addr, 0, 3, node_permissions);
   print_api_r(res);
   print_str("\n");

   phys_addr += SIZE_PAGE;

   print_str("load_page_table(enclave_id, phys_addr, 0, 2, node_permissions)");
   res = load_page_table(enclave_id, phys_addr, 0, 2, node_permissions);
   print_api_r(res);
   print_str("\n");

   phys_addr += SIZE_PAGE;

   print_str("load_page_table(enclave_id, phys_addr, 0, 1, node_permissions)");
   res = load_page_table(enclave_id, phys_addr, 0, 1, node_permissions);
   print_api_r(res);
   print_str("\n");

   phys_addr += SIZE_PAGE;

   dram_region_id_t code_id = 6;

   uintptr_t os_addr = DRAM_START + (code_id * SIZE_REGION);
   
   print_str("load_page(enclave_id, phys_addr, 0, os_addr, leaf_permissions)");
   res = load_page(enclave_id, phys_addr, 0, os_addr, leaf_permissions);
   print_api_r(res);
   print_str("\n");
   
   // TREAD
   
   print_str("enclave_metadata_pages(1) : ");
   ret = enclave_metadata_pages(1);
   print_int(ret);
   print_str("\n");

   thread_id_t thread_id = enclave_id + (ret * SIZE_PAGE);

   int64_t num_metadata_pages_per_regions = NUM_METADATA_PAGES_PER_REGION;

   print_str("load_thread(enclave_id, thread_id, entry_pc, entry_sp, trap_handler_entry, fault_stack_ptr)");
   res = load_thread(enclave_id, thread_id, 0, 0, trap_handler_entry, fault_stack_ptr);
   print_api_r(res);
   print_str("\n");
   
   print_str("init_enclave(enclave_id)");
   res = init_enclave(enclave_id);
   print_api_r(res);
   print_str("\n");

   print_str("enter_enclave");
   res = enter_enclave(enclave_id, thread_id);
   print_api_r(res);
   print_str("\n");
   
   tohost = TOHOST_CMD(0, 0, 0b01);
}



