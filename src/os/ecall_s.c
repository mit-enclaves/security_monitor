#include <errno.h> // TODO only include ENOSYS?
#include <ecall_s.h>
#include <sm_util/sm_util.h>

// SM CALLS FROM OS (these come from S-mode)

void ecall_from_s_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
  uintptr_t code = regs[17];
  uint64_t arg0 = regs[10];
  uint64_t arg1 = regs[11];
  uint64_t arg2 = regs[12];
  uint64_t arg3 = regs[13];
  uint64_t arg4 = regs[14];
  uint64_t arg5 = regs[15];

  uint64_t retval;

  switch(code) {
    // Enclaves
    case SM_ENCLAVE_CREATE:
      retval = ecall_create_enclave(arg0, arg1, arg2, arg3, (bool)arg4);
      break;

    case SM_ENCLAVE_DELETE:
      retval = ecall_delete_enclave(arg0);
      break;

    case SM_ENCLAVE_ENTER:
      retval = ecall_enter_enclave(arg0, arg1, regs);
      break;

    case SM_ENCLAVE_INIT:
      retval = ecall_init_enclave(arg0);
      break;

    case SM_ENCLAVE_LOAD_HANDLER:
      retval = ecall_load_trap_handler(arg0, arg1);
      break;

    case SM_ENCLAVE_LOAD_PAGE_TABLE:
      retval = ecall_load_page_table(arg0, arg1, arg2, (uint64_t) arg3, arg4);
      break;

    case SM_ENCLAVE_LOAD_PAGE:
      retval = ecall_load_page(arg0, arg1, arg2, arg3, arg4);
      break;

    case SM_ENCLAVE_METADATA_PAGES:
      retval = ecall_enclave_metadata_pages((int64_t) arg0);
      break;

    // Fields
    case SM_GET_PUBLIC_FIELD:
      retval = monitor_unsupported;
      break;

    // Mail
    case SM_MAIL_ACCEPT:
      retval = sm_mail_accept((mailbox_id_t) arg0, (enclave_id_t) arg1);
      break;

    case SM_MAIL_RECEIVE:
      retval = sm_mail_receive((mailbox_id_t) arg0, (uintptr_t) arg1);
      break;

    case SM_MAIL_SEND:
      retval = sm_mail_send((enclave_id_t) arg0, (mailbox_id_t) arg1, (uintptr_t) arg2);
      break;

    // Regions
    case SM_REGION_ASSIGN:
      retval = sm_region_assign( (dram_region_id_t) arg0, (enclave_id_t) arg1 );
      break;

    case SM_REGION_BLOCK:
      retval = sm_region_block( arg0 );
      break;

    case SM_REGION_FLUSH:
      retval = sm_region_flush();
      break;

    case SM_REGION_FREE:
      retval = sm_region_free( arg0 );
      break;

    case SM_REGION_METADATA_CREATE:
      retval = sm_region_metadata_create( arg0 );
      break;

    case SM_REGION_METADATA_PAGES:
      retval = sm_region_metadata_pages();
      break;

    case SM_REGION_METADATA_START:
      retval = sm_region_metadata_start();
      break;

    case SM_REGION_OWNER:
      retval = sm_region_owner( arg0 );
      break;

    case SM_REGION_STATE:
      retval = sm_region_state( (dram_region_id_t) arg0 );
      break;

    // Threads
    case SM_THREAD_ASSIGN:
      retval = sm_thread_assign( arg0, arg1 );
      break;

    case SM_THREAD_DELETE:
      retval = sm_thread_delete( arg0 );
      break;

    case SM_THREAD_LOAD:
      retval = sm_thread_load( arg0, arg1, arg2, arg3, arg4, arg5 );
      break;

    case SM_THREAD_METADATA_PAGES:
      retval = sm_thread_metadata_pages();
      break;

    // All other calls are unsupported
    default:
      retval = MONITOR_UNSUPPORTED;
      break;
  }

  // Populate return value
  regs[10] = retval;
}
