#include <errno.h> // TODO only include ENOSYS?
#include <api.h>

// SM CALLS FROM OS (these come from S-mode)

void ecall_from_s_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
   uintptr_t code = regs[17], arg0 = regs[10], arg1 = regs[11], arg2 = regs[12], arg3 = regs[13], arg4 = regs[14], arg5 = regs[15], retval;

   switch(code) {

      case SBI_SM_OS_BLOCK_DRAM_REGION:
         retval = os_block_dram_region((dram_region_id_t) arg0);
         break;
      case SBI_SM_OS_SET_DMA_RANGE:
         retval = monitor_unsupported;
         break;
      case SBI_SM_OS_DRAM_REGION_STATE:
         retval = dram_region_state((dram_region_id_t) arg0);
         break;
      case SBI_SM_OS_DRAM_REGION_OWNER:
         retval = dram_region_owner((dram_region_id_t) arg0);
         break;
      case SBI_SM_OS_ASSIGN_DRAM_REGION:
         retval = assign_dram_region((dram_region_id_t) arg0, (enclave_id_t) arg1);
         break;
      case SBI_SM_OS_FREE_DRAM_REGION:
         retval = free_dram_region((dram_region_id_t) arg0);
         break;
      case SBI_SM_OS_FLUSH_CACHED_DRAM_REGIONS:
         retval = monitor_unsupported;
         break;
      case SBI_SM_OS_CREATE_METADATA_REGION:
         retval = create_metadata_region((dram_region_id_t) arg0);
         break;
      case SBI_SM_OS_METADATA_REGION_PAGES:
         retval = metadata_region_pages();
         break;
      case SBI_SM_OS_METADATA_REGION_START:
         retval = metadata_region_start();
         break;
      case SBI_SM_OS_THREAD_METADATA_PAGES:
         retval = thread_metadata_pages();
         break;
      case SBI_SM_OS_ENCLAVE_METADATA_PAGES:
         retval = enclave_metadata_pages((int64_t) arg0);
         break;
      case SBI_SM_OS_CREATE_ENCLAVE:
         retval = create_enclave(arg0, arg1, arg2, arg3, (bool) arg4);
         break;
      case SBI_SM_OS_LOAD_PAGE_TABLE:
         retval = load_page_table(arg0, arg1, arg2, (uint64_t) arg3, arg4);
         break;
      case SBI_SM_OS_LOAD_PAGE:
         retval = load_page(arg0, arg1, arg2, arg3, arg4);
         break;
      case SBI_SM_OS_LOAD_THREAD:
         retval = load_thread(arg0, arg1, arg2, arg3, arg4, arg5);
         break;
      case SBI_SM_OS_ASSIGN_THREAD:
         retval = assign_thread(arg0, arg1);
         break;
      case SBI_SM_OS_INIT_ENCLAVE:
         retval = init_enclave(arg0);
         break;
      case SBI_SM_OS_ENTER_ENCLAVE:
         retval = enter_enclave(arg0, arg1, regs);
         break;
      case SBI_SM_OS_DELETE_THREAD:
         retval = delete_thread(arg0);
         break;
      case SBI_SM_OS_DELETE_ENCLAVE:
         retval = delete_enclave(arg0);
         break;
      case SBI_SM_OS_COPY_DEBUG_ENCLAVE_PAGE:
         retval = monitor_unsupported;
         break;
      case SBI_SM_ENCLAVE_FETCH_FIELD:
         retval = monitor_unsupported;
         break;

      default:
         retval = ENOSYS;
         break;
   }

   regs[10] = retval;
}
