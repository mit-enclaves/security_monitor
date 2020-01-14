#include <sm.h>

// Need to lock ALL of the sender enclave's metadata region, the recipient enclave's metadata region (if different), and the buffer region

api_result_t sm_mail_send (mailbox_id_t mailbox_id, enclave_id_t recipient, phys_ptr_t in_message) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - recipient must be either OWNER_UNTRUSTED or valid enclave
    - if the recipient is an enclave, it must be initialized
    - mailbox_id must be valid
    - the recipient mailbox must be expecting this sender
    - the recipient mailbox must be empty
    - the message buffer must fit entirely in one region
    - the message buffer region must belong to the caller
  */

  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;
  mailbox_t * mailbox;

  // <TRANSACTION>

  // recipient must be either OWNER_UNTRUSTED or valid enclave
  // Lock the recepient's regions
  if (recipient == OWNER_UNTRUSTED) {

    // mailbox_id must be valid
    if (mailbox_id >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    if (!lock_untrusted_state()) {
      return MONITOR_CONCURRENT_CALL;
    }

    mailbox = &sm->untrusted_mailboxes[mailbox_id];

  } else { // an enclave
    api_result_t result = lock_region_iff_valid_enclave(recipient);
    if ( MONITOR_OK != result ) {
      return result;
    }

    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(recipient);

    // mailbox_id must be valid
    if (mailbox_id >= enclave_metadata->num_mailboxes) {
      unlock_region(addr_to_region_id(recipient));
      return MONITOR_INVALID_VALUE;
    }

    // if the recipient is an enclave, it must be initialized
    if(enclave_metadata->init_state != ENCLAVE_STATE_INITIALIZED) {
      unlock_region( addr_to_region_id(recipient));
      return MONITOR_INVALID_STATE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];

  }

  // Lock the caller's region
  if((caller != recipient) && (addr_to_region_id(caller) != addr_to_region_id(recipient))) {

    if(caller == OWNER_UNTRUSTED) {
      if (!lock_untrusted_state()) {
        // Unlock recipient's region (the recipient is an enclave)
        unlock_region(addr_to_region_id(recipient));

        return MONITOR_CONCURRENT_CALL;
      }
    }
    else if (!lock_region( addr_to_region_id(caller))) {
      // Unlock recipient's region
      if (recipient == OWNER_UNTRUSTED) {
        unlock_untrusted_state();
      } else {
        unlock_region(addr_to_region_id(recipient));
      }
      return MONITOR_CONCURRENT_CALL;
    }
  }

  // the recipient mailbox must be expecting this sender
  // the recipient mailbox must be empty

  if ((mailbox->expected_sender != caller) || (mailbox->state != ENCLAVE_MAILBOX_STATE_EMPTY)) {
    if (recepient == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(recepient));
    }
    if (caller == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(caller));
    }
    return MONITOR_INVALID_STATE;
  }

  // the message buffer must fit entirely in one region
  // the message buffer region must belong to the caller

  if ((addr_to_region_id(in_message) != addr_to_region_id(in_message + sizeof(uint8_t) * MAILBOX_SIZE)) ||
    (sm_region_owner(addr_to_region_id(in_message)) != caller)) {

    if (recepient == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(recepient));
    }
    if (caller == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(caller));
    }
    return MONITOR_INVALID_STATE;
  }

  // Lock the buffer region
  if(!lock_region(addr_to_region_id(in_message))) {
    if (recepient == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(recepient));
    }
    if (caller == OWNER_UNTRUSTED) {
      unlock_untrusted_state();
    } else {
      unlock_region(addr_to_region_id(caller));
    }
    return MONITOR_CONCURRENT_CALL;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------

  // Copy the sender's Measurement
  if (caller == OWNER_UNTRUSTED) {
    memset(&mailbox->sender_measurement, 0x00, sizeof(hash_t) );

  } else { // an enclave
    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);

    memcpy(&mailbox->sender_measurement, &enclave_metadata->measurement, sizeof(hash_t));
  }

  // Copy the message
  memcpy(&mailbox->message, &in_message, sizeof(uint8_t) * MAILBOX_SIZE);

  // Update the mailbox's state
  mailbox->state = ENCLAVE_MAILBOX_STATE_FULL;

  // Release locks
  if ( recepient == OWNER_UNTRUSTED ) {
    unlock_untrusted_state();
  } else {
    unlock_region( addr_to_region_id(recepient) );
  }
  if ( caller == OWNER_UNTRUSTED ) {
    unlock_untrusted_state();
  } else {
    unlock_region( addr_to_region_id(caller) );
  }
  unlock_region(addr_to_region_id(in_message));
  // </TRANSACTION>

  return MONITOR_OK;
}
