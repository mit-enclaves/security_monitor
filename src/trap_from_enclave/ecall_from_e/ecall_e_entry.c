#include <errno.h> // TODO only include ENOSYS?
#include <ecall_e.h>
#include <sm_util/sm_util.h>

// SM CALLS FROM ENCLAVE (from U-mode, within an enclave)

SM_ETRAP void ecall_from_enclave_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
   uintptr_t code = regs[17], arg0 = regs[10], arg1 = regs[11], retval;

   switch(code) {

      case UBI_SM_ENCLAVE_BLOCK_DRAM_REGION:
         retval = ecall_enclave_block_dram_region((dram_region_id_t) arg0);
         break;
      case UBI_SM_ENCLAVE_CHECK_OWNERSHIP:
         retval = ecall_dram_region_check_ownership((dram_region_id_t) arg0);
         break;
      case UBI_SM_ENCLAVE_ACCEPT_THREAD:
         retval = ecall_accept_thread(arg0, arg1);
         break;
      case UBI_SM_ENCLAVE_EXIT_ENCLAVE:
         retval = ecall_exit_enclave(regs);
         break;
      case UBI_SM_ENCLAVE_GET_ATTESTATION_KEY:
         retval = ecall_get_attestation_key(arg0);
         break;
      case UBI_SM_ENCLAVE_ACCEPT_MESSAGE:
         retval = ecall_accept_message((mailbox_id_t) arg0, (enclave_id_t) arg1);
         break;
      case UBI_SM_ENCLAVE_READ_MESSAGE:
         retval = ecall_read_message((mailbox_id_t) arg0, (uintptr_t) arg1);
         break;
      case UBI_SM_ENCLAVE_SEND_MESSAGE:
         ;
         uintptr_t arg2 = regs[12];
         retval = ecall_send_message((enclave_id_t) arg0, (mailbox_id_t) arg1, (uintptr_t) arg2);
         break;

      default:
         retval = ENOSYS;
         break;
   }
   regs[10] = retval;
}
