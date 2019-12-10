#include <ecall_s.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>
#include <sha3/sha3.h>

api_result_t enclave_create (enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug) {
   // TODO: Check all arguments validity

   // Check that enclave_id is page alligned
   if(enclave_id % PAGE_SIZE) {
      return monitor_invalid_value;
   }

   dram_region_t * dram_region_info = &(SM_GLOBALS.regions[REGION_IDX((uintptr_t) enclave_id)]);

   if(!lock_acquire(dram_region_info->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an metadata region
   if(dram_region_info->type != metadata_region) {
      lock_release(dram_region_info->lock); // Release Lock
      return monitor_invalid_value;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) METADATA_PM_PTR(enclave_id);

   // Check metadata pages availability

   uint64_t num_metadata_pages = ecall_enclave_metadata_pages(mailbox_count);

   if((METADATA_IDX(enclave_id) + num_metadata_pages) >= ecall_metadata_region_pages()) {
      lock_release(dram_region_info->lock); // Release Lock
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      if((page_map[i] & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) {
         lock_release(dram_region_info->lock); // Release Lock
         return monitor_invalid_state;
      }
   }

   // Initiate the metadata page map

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
      page_map[i] |= (enclave_id << ENTRY_OWNER_ID_OFFSET)
         | (metadata_enclave & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1));
   }

   enclave_t *enclave = (enclave_t *) enclave_id;

   // Initialize the enclave metadata
   enclave->initialized = 0;
   enclave->debug = debug;
   enclave->thread_count = 0;
   enclave->dram_bitmap = 0;
   enclave->last_phys_addr_loaded = 0;
   enclave->evbase = ev_base;
   enclave->evmask = ev_mask;

   // Initialize mailboxes
   enclave->mailbox_count = mailbox_count;
   enclave->mailbox_array = (mailbox_t *) (enclave_id + sizeof(enclave_t));
   for(int i = 0; i < enclave->mailbox_count; i++) {
      (enclave->mailbox_array[i]).sender = 0;
      (enclave->mailbox_array[i]).has_message = false;
      for(int j = 0; j < MAILBOX_SIZE; j++) {
         (enclave->mailbox_array[i]).message[j] = 0;
      }
   }

   // Update measurement
   sha3_init(&(enclave->sha3_ctx), sizeof(hash_t));

   struct inputs_create_t inputs = {0};
   inputs.ev_base = ev_base;
   inputs.ev_mask = ev_mask;
   inputs.mailbox_count = mailbox_count;
   inputs.debug = debug;

   sha3_update(&(enclave->sha3_ctx), &(inputs), sizeof(struct inputs_create_t));

   lock_release(dram_region_info->lock); // Release Lock

   return monitor_ok;
}
