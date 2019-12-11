#include <sm.h>

TODO

api_result_t sm_mail_send (uint64_t mailbox_id, enclave_id_t recipient, phys_ptr_t in_message) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - recipient must be either OWNER_UNTRUSTED or valid enclave
    - if the recipient is an enclave, it msut be initialized
    - mailbox_id must be valid
    - the recipient mailbox must be expecting this sender
    - the recipient mailbox must be empty
    - the message buffer must fit entirely in one region
    - the message buffer region must belong to the caller
  */

  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[get_core_id()].owner;
  mailbox_t * mailbox;

  hash_t measurement;

  // <TRANSACTION>
  if (recipient == OWNER_UNTRUSTED) {
    if (mailbox_id >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    if ( !lock_untrusted_state() ) {
      return MONITOR_CONCURRENT_CALL;
    }

    mailbox = &sm->untrusted_mailboxes[mailbox_id];

  } else { // an enclave
    api_result_t result = lock_region_iff_valid_enclave( recipient );
    if ( MONITOR_OK != result ) {
      return result;
    }

    if ( mailbox_id >= enclave_metadata->num_mailboxes ) {
      return MONITOR_INVALID_VALUE;
    }

    enclave_t * enclave_metadata = (enclave_t *)(recipient);
    mailbox = &enclave_metadata->mailboxes[mailbox_id];


    enclave_t * enclave_metadata = (enclave_t *)(recipient);

    if ( !lock_region( addr_to_region_id(caller) ) ) {
      return MONITOR_CONCURRENT_CALL;
    }

    if (enclave_metadata->num_mailboxes >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];
  }




  if (caller == OWNER_UNTRUSTED) {
    memset( &measurement, 0x00, sizeof(measurement) );

  } else { // an enclave
    enclave_t * enclave_metadata = (enclave_t *)(caller);

    if ( !lock_region( addr_to_region_id(caller) ) ) {
      return MONITOR_CONCURRENT_CALL;
    }

    if (enclave_metadata->num_mailboxes >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  if (caller == OWNER_UNTRUSTED) {

  mailbox->state = ENCLAVE_MAILBOX_STATE_EMPTY;
  mailbox->expected_sender = expected_sender;

  // Release locks
  if ( new_owner == OWNER_UNTRUSTED ) {
    unlock_untrusted_state();
  } else {
    unlock_region( addr_to_region_id(caller) );
  }
  // </TRANSACTION>

  return MONITOR_OK;
}
