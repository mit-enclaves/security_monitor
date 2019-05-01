#include <clib/clib.h>
#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>
#include <sha3/sha3.h>

struct inputs_create_t{
   uintptr_t ev_base;
   uintptr_t ev_mask;
   uint64_t mailbox_count;
   bool debug;
};

api_result_t create_enclave(enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug) {
   // TODO: Check all arguments validity

   // Check that enclave_id is page alligned	
   if(enclave_id % SIZE_PAGE) {
      return monitor_invalid_value;
   }

   dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) enclave_id)]);	

   if(!aquireLock(dram_region_ptr->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock

   // Check that dram region is an metadata region
   if(dram_region_ptr->type != metadata_region) {
      releaseLock(dram_region_ptr->lock); // Release Lock
      return monitor_invalid_value;
   }

   metadata_page_map_t page_map = (metadata_page_map_t) dram_region_ptr;

   // Check metadata pages availability

   uint64_t num_metadata_pages = enclave_metadata_pages(mailbox_count);

   if((METADATA_IDX(enclave_id) + num_metadata_pages) >= metadata_region_pages()) {
      releaseLock(dram_region_ptr->lock); // Release Lock
      return monitor_invalid_value;
   }

   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      if((page_map[i] & ((1ul << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) { 
         releaseLock(dram_region_ptr->lock); // Release Lock
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

   releaseLock(dram_region_ptr->lock); // Release Lock

   return monitor_ok;
}

/*
api_result_t load_trap_handler(enclave_id_t enclave_id, uintptr_t phys_addr) {
   // TODO: Does phys_addr has to be alligned?
   
   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *)  enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(enclave->last_phys_addr_loaded <= phys_addr) {
      return monitor_invalid_value;
   }
   
   // Check that the handlers fit in a DRAM region owned by the enclave
   uintptr_t end_phys_addr = phys_addr + SIZE_ENCLAVE_HANDLER;
   
   for(uintptr_t addr = phys_addr; addr < end_phys_addr; addr += SIZE_REGION) {
      if(!owned(addr, enclave_id)){
         return monitor_invalid_state;
      }
   }

   // Check that end_phys_addr points into a DRAM region owned by the enclave
   if(!owned(end_phys_addr, enclave_id)){
      return monitor_invalid_state;
   }

   enclave->meparbase = phys_addr;
   enclave->meparmask = ~ ((1ul << intlog2(~(phys_addr ^ end_phys_addr))) - 1);
  
   // TODO: need to grab the DRAM regions locks

   // Copy the handlers
   memcpy((void *) phys_addr, (void *) GLOBAL_ENCLAVE_HANDLERS_ADDRESS, SIZE_ENCLAVE_HANDLER);

   // Update the measurement
   sha3_update(&(enclave->sha3_ctx), GLOBAL_ENCLAVE_HANDLERS_ADDRESS, SIZE_ENCLAVE_HANDLER);
}
*/

api_result_t load_page_table_entry(enclave_id_t enclave_id, uintptr_t phys_addr, 
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   // Check that phys_addr is page alligned
   if(phys_addr % SIZE_PAGE) {
      return monitor_invalid_value;
   }

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *)  enclave_id;

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      return monitor_invalid_state;
   }

   // Check that phys_addr is higher than the last physical address
   if(enclave->last_phys_addr_loaded <= phys_addr) {
      return monitor_invalid_value;
   }

   // Check virtual addr validity
   if(((virtual_addr & enclave->evmask) != enclave->evbase) ||
         (((virtual_addr + SIZE_PAGE) & enclave->evmask) != enclave->evbase)) {
      return monitor_invalid_value;
   }

   // Check that phys_addr points into a DRAM region owned by the enclave
   if(!owned(phys_addr, enclave_id)){
      return monitor_invalid_state;
   }

   // Initialize page table entry
   if(level == 3) {
      enclave->eptbr = phys_addr;
   }
   else {
      uintptr_t pte_base = enclave->eptbr;
      if(!owned(pte_base, enclave_id)){
         return monitor_invalid_state;
      }

      for(int i = 2; i >= level; i--) {
         uint64_t * pte_addr = (uint64_t *) ((uint64_t) pte_base +
            ((virtual_addr >> (PAGE_OFFSET + (PN_OFFSET * i))) & PN_MASK));

         if(i != level) {
            uint64_t pte_acl = (*pte_addr) & ACL_MASK;
            if(((pte_acl & PTE_V) == 0) ||
                  (((pte_acl & PTE_R) == 0) && ((pte_acl & PTE_W) == PTE_W)) || 
                  ((pte_acl & PTE_R) == PTE_R) || ((pte_acl & PTE_X) == PTE_X)    ) {
               return monitor_invalid_state;
            }

            pte_base = (((*pte_addr) >> PAGE_ENTRY_ACL_OFFSET) & PPNs_MASK) << PAGE_OFFSET;

            if(!owned(pte_base, enclave_id)){
               return monitor_invalid_state;
            }
         }
         else {
            *pte_addr = 0 |
               ((phys_addr >> PAGE_OFFSET) & (PPNs_MASK) << PAGE_ENTRY_ACL_OFFSET) |
               (acl & ACL_MASK);
         }
      }
   }

   enclave->last_phys_addr_loaded = phys_addr;
   
   return monitor_ok;
}

struct inputs_load_pt_t{
   uintptr_t virtual_addr;
   uint64_t level;
   uintptr_t acl;
};

api_result_t load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr, 
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl) {

   if(level > 3) {
      return monitor_invalid_value;
   }

   // Check that ACL is valid and is a leaf ACL
   if(((acl & PTE_V) == 0) ||
         (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W)) || 
         ((acl & PTE_R) == PTE_R) || ((acl & PTE_X) == PTE_X)    ) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, level, acl);

   if(ret != monitor_ok) {
      releaseLock(er_ptr->lock);
      return ret;
   }

   // Update measurement
   struct inputs_load_pt_t inputs = {0};
   inputs.virtual_addr = virtual_addr;
   inputs.level = level;
   inputs.acl = acl;
 
   enclave_t *enclave = (enclave_t *) enclave_id;
   sha3_update(&(enclave->sha3_ctx), &(inputs), sizeof(struct inputs_load_pt_t));

   releaseLock(er_ptr->lock);

   return monitor_ok;
}

struct inputs_load_page_t{
   uintptr_t virtual_addr;
   uintptr_t acl;
};

api_result_t load_page(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl) {

   // Check that ACL is valid anf is not a leaf ACL
   if(((acl & PTE_V) == 0) ||
         (((acl & PTE_R) == 0) && ((acl & PTE_W) == PTE_W)) || 
         (((acl & PTE_R) == 0) && ((acl & PTE_X) == 0))         ) {
      return monitor_invalid_value;
   }

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   // Load page table entry in page table and check arguments
   api_result_t ret = load_page_table_entry(enclave_id, phys_addr, virtual_addr, 0, acl); // TODO: Are loaded pages always kilo pages?

   if(ret != monitor_ok) {
      releaseLock(er_ptr->lock);
      return ret;
   }

   // Load page
   // TODO: Check os_addr
   memcpy((void *) phys_addr, (void *) os_addr, SIZE_PAGE);

   // Update measurement
   struct inputs_load_page_t inputs = {0};
   inputs.virtual_addr = virtual_addr;
   inputs.acl = acl;

   sha3_update(&(((enclave_t *) enclave_id)->sha3_ctx), &(inputs), sizeof(struct inputs_load_pt_t));
   sha3_update(&(((enclave_t *) enclave_id)->sha3_ctx), (const void *) os_addr, SIZE_PAGE);

   releaseLock(er_ptr->lock);

   return monitor_ok;
}

api_result_t init_enclave(enclave_id_t enclave_id) {

   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   // Check that the enclave is not initialized.
   if(enclave->initialized) {
      releaseLock(er_ptr->lock);
      return monitor_invalid_state;
   }

   // Initialize Enclave
   enclave->initialized = true;

   // Output the measurement
   sha3_final(&(enclave->measurement),&(enclave->sha3_ctx));

   releaseLock(er_ptr->lock);

   return monitor_ok;
}

api_result_t delete_enclave(enclave_id_t enclave_id) {
   
   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   // Check if enclave is has threads initialized
   if(enclave->thread_count != 0) {
      releaseLock(er_ptr->lock);
      return monitor_invalid_state;
   }

   // Free the DRAM regions
   
   for(int i = 0; i < NUM_REGIONS; i++) {
      
      if((enclave->dram_bitmap >> i) & 1ul) {
         
         // Get a pointer to the DRAM region datastructure
         dram_region_t *r_ptr = &(sm_globals.regions[i]);

         if(!aquireLock(r_ptr->lock)) {
            return monitor_concurrent_call;
         } // Acquire Lock

         if((r_ptr->owner != enclave_id) || 
               (r_ptr->type != enclave_region) ||
               (r_ptr->state != dram_region_blocked)) {
            releaseLock(r_ptr->lock);
            releaseLock(er_ptr->lock);
            return monitor_invalid_state;
         }
         
         // TODO: zero the region?

         releaseLock(r_ptr->lock);

         api_result_t ret = free_dram_region((dram_region_id_t) i);

         if(ret != monitor_ok) {
            releaseLock(er_ptr->lock);
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

   uint64_t num_metadata_pages = enclave_metadata_pages(mailbox_count);
   
   metadata_page_map_t page_map = (metadata_page_map_t) er_ptr;
   
   for(int i = METADATA_IDX(enclave_id);
         i < (METADATA_IDX(enclave_id) + num_metadata_pages);
         i++) {
      page_map[i] = 0;
   }

   releaseLock(er_ptr->lock);

   return monitor_ok;
}


api_result_t enter_enclave(enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs) {
   
   // Check if enclave_id is valid
   if(!is_valid_enclave(enclave_id)) {
      return monitor_invalid_value;
   }

   enclave_t * enclave = (enclave_t *) enclave_id;

   // Get a pointer to the DRAM region datastructure of the enclave metadata and aquire the lock
   dram_region_t *er_ptr = &(sm_globals.regions[REGION_IDX(enclave_id)]);

   if(!aquireLock(er_ptr->lock)) {
      return monitor_concurrent_call;
   }

   // Check that the enclave is initialized.
   if(!enclave->initialized) {
      releaseLock(er_ptr->lock);
      return monitor_invalid_state;
   }

   releaseLock(er_ptr->lock);
   
   // Get the curent core metadata and aquire its lock
   core_t *core = &(sm_globals.cores[read_csr(mhartid)]);
   
   if(!aquireLock(core->lock)) {
      return monitor_concurrent_call;
   } // Acquire Lock
   
   // Get a pointer to the DRAM region datastructure of the thread metadata and aquire the lock
   dram_region_t * tr_ptr = &(sm_globals.regions[REGION_IDX((uintptr_t) thread_id)]);
   
   if(!aquireLock(tr_ptr->lock)) {
      releaseLock(core->lock);
      return monitor_concurrent_call;
   } // Acquire Lock
   
   thread_t *thread = (thread_t *) thread_id;
  
   // Check thread_id validity   
   api_result_t ret = is_valid_thread(enclave_id, thread_id);
   
   if(ret != monitor_ok) {
      releaseLock(core->lock);
      releaseLock(tr_ptr->lock); // Release Lock
      return ret;
   }
   
   // Check that the thread is not already sheduled
   if(!aquireLock(thread->is_scheduled)) {
      releaseLock(core->lock);
      releaseLock(tr_ptr->lock); // Release Lock
      return monitor_invalid_state;
   }

   bool aex = thread->aex_present;
   
   // Save untrusted state, initialize enclave state
   for(int i = 0; i < NUM_REGISTERS; i++) {
      thread->untrusted_state[i] = regs[i];
      regs[i] = aex ? thread->aex_state[i] : 0; // TODO: Init registers?
   }

   // Save untrusted sp, pc
   thread->untrusted_sp = regs[2];
   thread->untrusted_pc = read_csr(mepc);

   // Set the enclave sp and pc
   regs[2] = thread->entry_sp;
   write_csr(mepc, thread->entry_pc);

   if(aex) {
      thread->aex_present = false;
   }

   releaseLock(tr_ptr->lock); // Release Lock

   // Update the core's metadata
   core->owner = enclave_id;
   core->has_enclave_schedule = true;
   core->cur_thread = thread_id;

   releaseLock(core->lock);

   return monitor_ok;
}
