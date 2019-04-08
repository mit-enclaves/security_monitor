#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <sm_util/sm_util.h>

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
	if((dram_region_ptr->type != metadata_region) 
		releaseLock(dram_region_ptr->lock); // Release Lock
		return monitor_invalid_value;
	}


	metadata_page_map_t page_map = (metadata_page_map_t) dram_region_ptr;

	// Check metadata pages availability
	
	uint64_t num_metadata_pages = enclave_metadata_pages(mailbox_count);

	if((METADATA_IDX(enclave_id) + num_metadata_pages) >= NUM_METADATA_PAGES_PER_REGION) {
		releaseLock(dram_region_ptr->lock); // Release Lock
		return monitor_invalid_value;
	}

	for(int i = METADATA_IDX(enclave_id);
	       	i < (METADATA_IDX(enclave_id) + num_metadata_pages);
	       	i++) {
		if((page_map[i] & ((1u << ENTRY_OWNER_ID_OFFSET) - 1)) != metadata_free) { 
			releaseLock(dram_region_ptr->lock); // Release Lock
			return monitor_invalid_state;
		}
	}

	// Initiate the metadata page map
		
	for(int i = METADATA_IDX(enclave_id);
	       	i < (METADATA_IDX(enclave_id) + num_metadata_pages);
	       	i++) {
		page_map[i] = 0;
		page_map[i] |= (enclave_id << ENTRY_OWNER_ID_OFFSET) | (metadata_enclave & ((1u << ENTRY_OWNER_ID_OFFSET) - 1))
	}
	
	enclave_t *enclave = (enclave_t *) enclave_id;

	enclave->initialized = 0;
	enclave->debug = debug;
	enclave->thread_cout = 0;
	enclave->dram_bitmap = 0; // TODO initialize bitmap
	enclave->measurement = 0;
	enclave->mailbox_count = mailbox_count;
	enclave->mailbox_array = 0; // TODO initialize mailboxes

	releaseLock(dram_region_ptr->lock); // Release Lock
	
	return monitor_ok;
}
