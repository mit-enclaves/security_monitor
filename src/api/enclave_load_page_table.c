#include <sm.h>

TODO

api_result_t sm_enclave_load_page_table (enclave_id_t enclave_id, phys_ptr_t phys_addr,
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   if(level > 3) {
      return monitor_invalid_value;
   }

   // Check that ACL is valid
   if(((acl & PTE_V) == 0) ||
         (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W))) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, level, acl);

   if(ret != monitor_ok) {
      lock_release(er_info->lock);
      return ret;
   }

   // Update measurement
   struct inputs_load_pt_t inputs = {0};
   inputs.virtual_addr = virtual_addr;
   inputs.level = level;
   inputs.acl = acl;

   enclave_metadata_t *enclave = (enclave_metadata_t *) enclave_id;
   sha3_update(&(enclave->sha3_ctx), &(inputs), sizeof(struct inputs_load_pt_t));

   lock_release(er_info->lock);

   return monitor_ok;
}
