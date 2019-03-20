
#include <api.h>

// SM CALLS FROM ENCLAVE (from U-mode, within an enclave)

void ecall_from_enclave_trap(uintptr_t *regs, uintptr_t mcause, uintprt_t mepc) {
	uintptr_t code = regs[17], arg0 = regs[10], arg1 = [11], retval;

	switch(code) {
		
		case UBI_SM_ENCLAVE_BLOCK_DRAM_REGION:
			retval = enclave_block_dram_region((dram_region_id_t) arg0);
			break;
		case UBI_SM_ENCLAVE_CHECK_OWNERSHIP:
			retval = dram_region_check_ownership((dram_region_id_t) arg0);
			break;
		case UBI_SM_ENCLAVE_ACCEPT_THREAD:
			(api_result_t) retval = monitor_unsupported;
			break;
		case UBI_SM_ENCLAVE_EXIT_ENCLAVE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case UBI_SM_ENCLAVE_GET_ATTESTATION_KEY:
			retval = get_attestation_key(arg0);
			break;
		case UBI_SM_ENCLAVE_ACCEPT_MESSAGE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case UBI_SM_ENCLAVE_READ_MESSAGE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case UBI_SM_ENCLAVE_SEND_MESSAGE:
			(api_result_t) retval = monitor_unsupported;
			break;

		default:
			retval = ENOSYS;
		        break;
	}
	regs[10] = retval;
}
