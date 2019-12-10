#include <sm.h>

api_result_t sm_enclave_delete (enclave_id_t enclave_id) {

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_info = &(SM_GLOBALS.regions[REGION_IDX(enclave_id)]);

   if(!lock_acquire(er_info->lock)) {
      return monitor_concurrent_call;
   }

   // Check if enclave is has threads initialized
   if(enclave->thread_count != 0) {
      lock_release(er_info->lock);
      return monitor_invalid_state;
   }

   // Free the DRAM regions

   for(int i = 0; i < NUM_REGIONS; i++) {

      if((enclave->dram_bitmap >> i) & 1ul) {

         // Get a pointer to the DRAM region datastructure
         dram_region_t *r_info = &(SM_GLOBALS.regions[i]);

         if(!lock_acquire(r_info->lock)) {
            return monitor_concurrent_call;
         } // Acquire Lock

         if((r_info->owner != enclave_id) ||
               (r_info->type != enclave_region) ||
               (r_info->state != dram_region_blocked)) {
            lock_release(r_info->lock);
            lock_release(er_info->lock);
            return monitor_invalid_state;
         }

         // TODO: zero the region?

         lock_release(r_info->lock);

         api_result_t ret = ecall_free_dram_region((dram_region_id_t) i);

         if(ret != monitor_ok) {
            lock_release(er_info->lock);
            return ret;
         }

         else {
            enclave->dram_bitmap &= ~(1ul << i);
         }
      }
   }

   // Clean the metadata

   uint64_t mailbox_count = enclave->mailbox_count;
   uint64_t size_e = sizeof(enclave_t) + (sizeof(mailbox_t) * mailbox_count);

   memset((void *) enclave_id, 0, size_e);

   // Clean the metadata page map

   uint64_t num_metadata_pages = ecall_enclave_metadata_pages(mailbox_count);

   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(enclave_id);

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
   }

   lock_release(er_info->lock);

   return monitor_ok;
}
