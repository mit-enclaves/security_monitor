#include <api.h>

// SM CALLS FROM OS (these come from S-mode)

void ecall_from_s_trap(uintptr_t *regs, uintptr_t mcause, uintprt_t mepc) {
	uintptr_t code = regs[17], arg0 = regs[10], arg1 = [11], retval;

	switch(code) {
		 
		case SBI_SM_OS_BLOCK_DRAM_REGION:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_SET_DMA_RANGE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_DRAM_REGION_STATE:
			retval = dram_region_state((size_t) arg0);
			break;
		case SBI_SM_OS_DRAM_REGION_OWNER:
			retval = dram_region_owner((size_t) arg0);
			break;
		case SBI_SM_OS_ASSIGN_DRAM_REGION:
			retval = assign_dram_region((size_t) arg0, (enclave_id_t) arg1);
			break;
		case SBI_SM_OS_FREE_DRAM_REGION:
			retval = free_dram_region((size_t) arg0);
			break;
		case SBI_SM_OS_FLUSH_CACHED_DRAM_REGIONS:
			retval = flush_cached_dram_regions();
			break;
		case SBI_SM_OS_METADATA_REGION_PAGES:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_METADATA_REGION_START:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_THREAD_METADATA_PAGES:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_ENCLAVE_METADATA_PAGES:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_CREATE_ENCLAVE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_LOAD_PAGE_TABLE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_LOAD_PAGE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_LOAD_THREAD:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_ASSIGN_THREAD:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_INIT_ENCLAVE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_ENTER_ENCLAVE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_DELETE_THREAD:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_DELETE_ENCLAVE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_OS_COPY_DEBUG_ENCLAVE_PAGE:
			(api_result_t) retval = monitor_unsupported;
			break;
		case SBI_SM_ENCLAVE_FETCH_FIELD:
			(api_result_t) retval = monitor_unsupported;
			break;

		default:
			retval = ENOSYS;
		        break;
	}

	regs[10] = retval;
}
