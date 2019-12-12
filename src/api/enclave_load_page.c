#include <sm.h>

TODO 

api_result_t sm_enclave_load_page (enclave_id_t enclave_id, phys_ptr_t phys_addr,
      uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl) {

   // Check that ACL is valid and is a leaf ACL
   if(((acl & PTE_V) == 0) ||
         (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W)) ||
         (((acl & PTE_R) == 0) && ((acl & PTE_X) == 0))         ) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, 0, acl); // TODO: Are loaded pages always kilo pages?

   if(ret != monitor_ok) {
      lock_release(er_info->lock);
      return ret;
   }

   // Load page
   // TODO: Check os_addr
   memcpy((void *) phys_addr, (void *) os_addr, PAGE_SIZE);

   // Update measurement
   struct inputs_load_page_t inputs = {0};
   inputs.virtual_addr = virtual_addr;
   inputs.acl = acl;

   sha3_update(&(((enclave_metadata_t *) enclave_id)->sha3_ctx), &(inputs), sizeof(struct inputs_load_pt_t));
   sha3_update(&(((enclave_metadata_t *) enclave_id)->sha3_ctx), (const void *) os_addr, PAGE_SIZE);

   lock_release(er_info->lock);

   return monitor_ok;
}
