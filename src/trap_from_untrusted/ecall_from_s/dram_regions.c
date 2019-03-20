#include <api.h>
#include <sm.h>
#include <csr.h>

dram_region_state_t dram_region_state(dram_region_id_t id) {
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // TODO: reclaim lock

	dram_region_type_s state = r_ptr->state;
	
	releaseLock(r_ptr->lock); // TODO: release lock
	
	return state; 
}

enclave_id_t dram_region_owner(dram_region_id_t id) {
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // TODO: reclaim lock

	enclave_id_t owner = r_ptr->owner;
	
	releaseLock(r_ptr->lock); // TODO: release lock
	
	return owner; 
}

api_result_t assign_dram_region(dram_region_id_t id, enclave_id_t new_owner) {
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // TODO: reclaim lock
	
	if(r_ptr->state != dram_region_free) {
		return monitor_invalid_state;
	}

	if(r_ptr->type == untrusted_region) {
		XLENINT mmrbm = read_csr(CSR_MMRBM);
		mmrbm &= ~(1u << id);
		write_csr(CSR_MMRBM, mmrbm);
	}
	else if(r_ptr->type == enclave_region) {
		XLENINT memrbm = r_ptr->owner->dram_bitmap;
		memrbm &= ~(1u << id);
		r_ptr->owner->dram_bitmap = memrbm;
	}

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
	
	r->state = dram_region_owned;

	releaseLock(r_ptr->lock); // TODO: release lock

	return monitor_ok;
}

api_result_t block_dram_region(dram_region_id_t id) {
	if(id < NUM_REGIONS) {
		return monitor_invalid_value;
	}
	
	dram_region_t *r_ptr = &(sm_globals.regions[id]);
	
	if(!reclaimLock(r_ptr->lock)) {
		return monitor_concurrent_call;
	} // TODO: reclaim lock
	
	if(r_ptr->state != dram_region_owned) {
		return monitor_invalid_state;
	}

	if((r_ptr->type != untrusted_region)) {
		return monitor_access_denied;
	}

	r_ptr->state = dram_region_blocked;

	releaseLock(r_ptr->lock); // TODO: release lock

	return monitor_ok;
}

api_result_t free_dram_region(dram_region_id_t id);

api_result_t s
