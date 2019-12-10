#include <data_structures.h>
#include <sm.h>
#include <sm_util/sm_util.h>

SM_UTRAP bool is_valid_enclave(enclave_id_t enclave_id) {

   // Check that enclave_id is page alligned
   if(enclave_id % PAGE_SIZE) {
      return false;
   }

   dram_region_t * dram_region_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) enclave_id)]);

   if(!lock_acquire(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an metadata region
   if(dram_region_ptr->type != metadata_region) {
      lock_release(dram_region_ptr->lock); // Release Lock
      return false;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(enclave_id);
   metadata_page_map_entry_t entry = page_map[METADATA_IDX(enclave_id)];

   // Check that metadata entry is owned by the right enclave
   if((entry >> ENTRY_OWNER_ID_OFFSET) != enclave_id) {
      lock_release(dram_region_ptr->lock); // Release Lock
      return false;
   }

   // Check that metadata entry is of type enclave
   if((entry & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_enclave) {
      lock_release(dram_region_ptr->lock); // Release Lock
      return false;
   }

   lock_release(dram_region_ptr->lock); // Release Lock

   return true;
}

SM_UTRAP bool owned(uintptr_t phys_addr, enclave_id_t enclave_id) {

   dram_region_t * dram_region_ptr = &(SM_GLOBALS.regions[REGION_IDX(phys_addr)]);

   if(!lock_acquire(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an enclave region and is owned by the given enclave
   if((dram_region_ptr->type != enclave_region)
         || (dram_region_ptr->state != dram_region_owned)
         || (dram_region_ptr->owner != enclave_id)) {
      lock_release(dram_region_ptr->lock); // Release Lock
      return false;
   }

   lock_release(dram_region_ptr->lock); // Release Lock

   return true;
}

SM_UTRAP bool check_buffer_ownership(uintptr_t buff_phys_addr, size_t size_buff, enclave_id_t enclave_id) {

   // Check that the buffer is contained in a memory regions owned by the enclave.
   return owned(buff_phys_addr, enclave_id) && owned(buff_phys_addr + size_buff * 8, enclave_id);
}

SM_UTRAP api_result_t is_valid_thread(enclave_id_t enclave_id, thread_id_t thread_id) {

   if(thread_id % PAGE_SIZE) {
      return monitor_invalid_value;
   }

   dram_region_t * tr_ptr = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) thread_id)]);

   // Check that dram region is an metadata region
   if(tr_ptr->type != metadata_region) {
      return monitor_invalid_state;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(thread_id);

   uint64_t num_metadata_pages = ecall_thread_metadata_pages();

   if((METADATA_IDX(thread_id) + num_metadata_pages) >= ecall_metadata_region_pages()) {
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(thread_id);
         i < (METADATA_IDX(thread_id) + num_metadata_pages);
         i++) {
      if((page_map[i] >> ENTRY_OWNER_ID_OFFSET) != enclave_id) {
         return monitor_invalid_state;
      }
      if((page_map[i] & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_thread) {
         return monitor_invalid_state;
      }
   }

   return monitor_ok;
}

SM_UTRAP uint64_t ecall_metadata_region_pages() {
   return NUM_METADATA_PAGES_PER_REGION;
}

SM_UTRAP uint64_t ecall_metadata_region_start() {
   uint64_t size_mpm = sizeof(metadata_page_map_entry_t) * ecall_metadata_region_pages();
   return (size_mpm / PAGE_SIZE) + ((size_mpm % PAGE_SIZE) ? 1 : 0); // Rouded up division. No +1 as indexing starts at 0
}

SM_UTRAP uint64_t ecall_thread_metadata_pages() {
   uint64_t size_th = sizeof(thread_t);
   return (size_th / PAGE_SIZE) + ((size_th % PAGE_SIZE) ? 1 : 0); // Rouded up division
}

SM_UTRAP uint64_t ecall_enclave_metadata_pages(uint64_t mailbox_count) {
   uint64_t size_e = sizeof(enclave_t) + (sizeof(mailbox_t) * mailbox_count);
   return (size_e / PAGE_SIZE) + ((size_e % PAGE_SIZE) ? 1 : 0); // Rouded up division
}
