#include <api.h>
#include <sm.h>
#include <csr/csr.h>
#include <clib/clib.h> // TODO: only include memcpy?

#include <boot_api.h> //TODO: Is this the right way to do it?

#define REGION_IDX(addr) ((1u<<(addr >> intlog2(SIZE_DRAM / NUM_REGIONS))) & (NUM_REGIONS - 1))

inline bool owned(uintptr_t phys_addr, enclave_id_t enclave_id) {

	dram_region_t * dram_region_ptr = &(sm_globals.regions[REGION_IDX(phys_addr)]);	

	// Check that dram region is an enclave region and is owned by the given enclave
	if((dram_region_ptr->type != enclave_region) 
			|| (dram_region_ptr->state != dram_region_owned) 
			|| (dram_region_ptr->owner != enclave_id)) {
		return false;
	}

	return true;
}

inline bool check_buffer_ownership(uintptr_t buff_phys_addr, size_t size_buff, enclave_id_t enclave_id) {
	
	// Check that the buffer is contained in a memory regions owned by the enclave.
	return owned(buff_phys_addr, enclave_id) && owned(buff_phys_addr + size_buff * 8, enclave_id);
}


api_result_t get_attestation_key(uintptr_t phys_addr) {
	// Check that the caller is an attestation enclave
	enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;
	if(((enclave_t *) caller_id)->measurement != sm_globals.signing_enclave_measurement) {
		return monitor_access_denied;
	}

	size_t size_key = sizeof(uint8_t) * 64;

	// Check phys_addr validity
	if(!check_buffer_ownership(phys_addr, size_key, caller_id)) {
		return monitor_invalid_value;
	}

	memcpy((void *) phys_addr, boot_api_state.security_monitor_secret_key, size_key);

	return monitor_ok;
}

api_result_t accept_message(mailbox_id_t mailbox_id, enclave_id_t expected_sender) {
	// Check that caller is an enclave
	if(!sm_globals.cores[read_csr(mhartid)].has_enclave_schedule) {
		return monitor_invalid_state;	
	}
	
	enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;
	
	// Check mailbox_id validity
  	if(mailbox_id >= ((enclave_t *) caller_id)->mailbox_count) {
		return monitor_invalid_value;
	}	
	
	// Check that expected_sender is page alligned
	if((expected_sender & (SIZE_PAGE - 1)) != 0) {
		return monitor_invalid_value;
	}

	// Check that expected_sender corresponds to the given enclave
	if(!owned((uintptr_t) expected_sender, expected_sender)){
		return monitor_invalid_value;
	}

	// TODO :Check that expected_sender points to a metadata region
	
	mailbox_t *mailbox = &(((enclave_t *) caller_id)->mailbox_array[mailbox_id]);

	for(int i = 0; i < MAILBOX_SIZE; i++) {
		mailbox->message[i] = 0;
	}

	mailbox->has_message = false;

	mailbox->sender = expected_sender;

	return monitor_ok;
}

api_result_t read_message(mailbox_id_t mailbox_id, uintptr_t phys_addr) {
	// Check that caller is an enclave
	if(!sm_globals.cores[read_csr(mhartid)].has_enclave_schedule) {
		return monitor_invalid_state;	
	}

	enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;

	// Check mailbox_id validity
  	if(mailbox_id >= ((enclave_t *) caller_id)->mailbox_count) {
		return monitor_invalid_value;
	}	
	
	// Check that the buffer is contained in a memory regions accessible by the enclave.
	if(!check_buffer_ownership(phys_addr, sizeof(uint8_t) * MAILBOX_SIZE + sizeof(hash_t), caller_id)) {
		return monitor_invalid_value;
	}
	
	mailbox_t *mailbox = &(((enclave_t *) caller_id)->mailbox_array[mailbox_id]);

	// Copy message form mailbox to buffer
	memcpy((void *) phys_addr, mailbox->message, sizeof(uint8_t) * MAILBOX_SIZE);

	// Copy sender measurement into buffer
	memcpy((void *) phys_addr + sizeof(uint8_t) * MAILBOX_SIZE, ((enclave_t *) mailbox->sender)->measurement, sizeof(hash_t));

	return monitor_ok;
}

api_result_t send_message(enclave_id_t enclave_id, mailbox_id_t mailbox_id, uintptr_t phys_addr) {
	// Check that caller is an enclave
	if(!sm_globals.cores[read_csr(mhartid)].has_enclave_schedule) {
		return monitor_invalid_state;	
	}

	enclave_id_t caller_id = sm_globals.cores[read_csr(mhartid)].owner;

	// Check mailbox_id validity
  	if(mailbox_id >= ((enclave_t *) caller_id)->mailbox_count) {
		return monitor_invalid_value;
	}	

	// Check that enclave_id is page alligned
	if((enclave_id & (SIZE_PAGE - 1)) != 0) {
		return monitor_invalid_value;
	}

	// Check that enclave_id corresponds to the given enclave
	if(!owned((uintptr_t) enclave_id, enclave_id)){
		return monitor_invalid_value;
	}

	// TODO :Check that enclave_id points to a metadata region
	
	// Check that the buffer is contained in a memory regions owned by the enclave.
	if(!check_buffer_ownership(phys_addr, sizeof(uint8_t) * MAILBOX_SIZE, caller_id)) {
		return monitor_invalid_value;
	}

	mailbox_t *mailbox = &(((enclave_t *) enclave_id)->mailbox_array[mailbox_id]);

	if(mailbox->sender != caller_id) {
		return monitor_invalid_state; 
	}
	if(mailbox->has_message) {
		return monitor_invalid_state; 
	}

	// TODO copy hash f sender
	
	memcpy(mailbox->message, (const void *) phys_addr, sizeof(uint8_t) * MAILBOX_SIZE);

	mailbox->has_message = true;

	return monitor_ok;
}
