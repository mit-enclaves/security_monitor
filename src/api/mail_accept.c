#include <sm.h>

api_result_t sm_internal_mail_accept (mailbox_id_t mailbox_id, enclave_id_t expected_sender) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - mailbox_id must be valid
    NOTE: expected_sender is unauthenticated
  */

  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;
  mailbox_t * mailbox;

  // <TRANSACTION>
  if (caller == OWNER_UNTRUSTED) {
    if (mailbox_id >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    if ( !lock_untrusted_state() ) {
      return MONITOR_CONCURRENT_CALL;
    }

    mailbox = &sm->untrusted_mailboxes[mailbox_id];

  } else { // an enclave
    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);

    if ( !lock_region( addr_to_region_id(caller) ) ) {
      return MONITOR_CONCURRENT_CALL;
    }

    if (mailbox_id >= enclave_metadata->num_mailboxes) {
      return MONITOR_INVALID_VALUE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------
  mailbox->state = ENCLAVE_MAILBOX_STATE_EMPTY;
  mailbox->expected_sender = expected_sender;

  // Release locks
  if ( caller == OWNER_UNTRUSTED ) {
    unlock_untrusted_state();
  } else {
    unlock_region( addr_to_region_id(caller) );
  }
  // </TRANSACTION>

  return MONITOR_OK;
}
