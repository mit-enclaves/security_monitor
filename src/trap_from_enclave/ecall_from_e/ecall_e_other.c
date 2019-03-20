#include <api.h>
#include <sm.h>
#include <csr.h>

api_result_t get_attestation_key(uintptr_t phys_addr) {
	// Check argument validity
	// TODO: Check validity phys_addr
	
	// Check that the caller is an attestation enclave
	enclave_id_t caller_id = sm_globals.cores[csr_read(mhartid)].owner
	if(caller_id->measurement != sm_globals.signing_enclave_measurement) {
		releaseLock(r_ptr->lock); // TODO: release lock
		return monitor_access_denied;
	}

	// TODO copy the keys

	releaseLock(r_ptr->lock); // TODO: release lock

	return monitor_ok;
}
