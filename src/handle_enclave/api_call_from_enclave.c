#include <sm.h>
#include "handle_enclave/handles_enclave.h"

// SM API CALLS FROM ENCLAVE (from U-mode, within an enclave)

void ecall_from_enclave_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
  uintptr_t code = regs[17];

  if((code >= KERNEL_ECALL_CODE_MIN) && (code <= KERNEL_ECALL_CODE_MAX)) { // Kernel ecall

#if (DEBUG_ENCLAVE == 1)
    delegate_ecall_to_kernel(regs, mcause, mepc);
#endif

  } else { // SM ecall


    uint64_t arg0 = regs[10];
    uint64_t arg1 = regs[11];
    uint64_t arg2 = regs[12];
    uint64_t arg3 = regs[13];

    uint64_t retval;

    switch(code) {
      // Enclaves
      case SM_ENCLAVE_EXIT:
        retval = sm_internal_enclave_exit(regs);
        break;

      case SM_ENCLAVE_GET_KEYS:
        retval = sm_internal_enclave_get_keys( arg0, arg1, arg2, arg3 );
        break;
      
        // Fields
      case SM_GET_PUBLIC_FIELD:
        retval = sm_internal_get_public_field(arg0, (uintptr_t) arg1);
        break;

        // Mail
      case SM_MAIL_ACCEPT:
        retval = sm_internal_mail_accept((mailbox_id_t) arg0, (enclave_id_t) arg1);
        break;

      case SM_MAIL_RECEIVE:
        retval = sm_internal_mail_receive((mailbox_id_t) arg0, (uintptr_t) arg1, (uintptr_t) arg2);
        break;

      case SM_MAIL_SEND:
        retval = sm_internal_mail_send((enclave_id_t) arg0, (mailbox_id_t) arg1, (uintptr_t) arg2);
        break;

        // Regions
      case SM_REGION_BLOCK:
        retval = sm_internal_region_block((region_id_t) arg0);
        break;

      case SM_REGION_CHECK_OWNED:
        retval = sm_internal_region_check_owned((region_id_t) arg0);
        break;

        // All other calls are unsupported
      default:
        retval = MONITOR_UNSUPPORTED;
        break;
    }

    // Populate return value
    regs[10] = retval;
  }
}
