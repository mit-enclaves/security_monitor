#include <api.h>
#include <sm.h>
#include <csr.h>

dram_region_state_t dram_region_state(dram_region_id_t id) {
	// Check argument validity
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	// Get a pointer to the DRAM region datastructure	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // Acquire Lock

	dram_region_type_s state = r_ptr->state;
	
	releaseLock(r_ptr->lock); // Release Lock
	
	return state; 
}

enclave_id_t dram_region_owner(dram_region_id_t id) {
	// Check argument validity
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	// Get a pointer to the DRAM region datastructure	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // Acquire Lock

	enclave_id_t owner = r_ptr->owner;
	
	releaseLock(r_ptr->lock); // Release Lock
	
	return owner; 
}

api_result_t assign_dram_region(dram_region_id_t id, enclave_id_t new_owner) {
	// Check arguments validity
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}

	// TODO: what is a valid argument for new owner
	
	// Get a pointer to the DRAM region datastructure	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // Acquire Lock

	// The DRAM region must be free	
	if(r_ptr->state != dram_region_free) {
		releaseLock(r_ptr->lock); // Release Lock
		return monitor_invalid_state;
	}

	// If the DRAM region belongs to the OS
	// remove it from the OS bitmap
	if(r_ptr->type == untrusted_region) {
		XLENINT mmrbm = read_csr(CSR_MMRBM);
		mmrbm &= ~(1u << id);
		write_csr(CSR_MMRBM, mmrbm);
	}
	// If the DRAM region belongs to an enclave
	// remove it from the enclave bitmap
	else if(r_ptr->type == enclave_region) {
		XLENINT memrbm = r_ptr->owner->dram_bitmap;
		memrbm &= ~(1u << id);
		r_ptr->owner->dram_bitmap = memrbm;
	}

	// Set the new owner and update the owner's bitmap
	if(new_owner == 0) {
		r->owner = 0;
		r->type  = untrusted_region;
		XLENINT mmrbm = read_csr(CSR_MMRBM);
		mmrbm |= (1u << id);
		write_csr(CSR_MMRBM, mmrbm);
	}
	else {
		r->owner = new_owner;
		r->type  = enclave_region; 
		XLENINT memrbm = r_ptr->owner->dram_bitmap;
		memrbm |= (1u << id);
		r_ptr->owner->dram_bitmap = memrbm;
	}
	
	// Update the DRAM region state
	r->state = dram_region_owned;

	releaseLock(r_ptr->lock); // Release Lock

	return monitor_ok;
}

api_result_t os_block_dram_region(dram_region_id_t id) {
	// Check argument validity
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	// Get a pointer to the DRAM region datastructure	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // Acquire Lock

	// The DRAM region must be owned
	if(r_ptr->state != dram_region_owned) {
		releaseLock(r_ptr->lock); // Release Lock
		return monitor_invalid_state;
	}

	// This handler only handle OS-owned regions
	if((r_ptr->type != untrusted_region)) {
		releaseLock(r_ptr->lock); // Release Lock
		return monitor_access_denied;
	}

	// Update the DRAM region state
	r_ptr->state = dram_region_blocked;

	releaseLock(r_ptr->lock); // Release Lock

	return monitor_ok;
}

api_result_t free_dram_region(dram_region_id_t id) {
	// Check argument validity
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	// Get a pointer to the DRAM region datastructure	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // Acquire Lock

	// The DRAM region must be blocked
	if(r_ptr->state != dram_region_blocked) {
		releaseLock(r_ptr->lock); // Release Lock
		return monitor_invalid_state;
	}

	// Update the DRAM region state
	r_ptr->state = dram_region_free;

	releaseLock(r_ptr->lock); // Release Lock

	return monitor_ok;
}
