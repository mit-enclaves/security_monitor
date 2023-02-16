#include <sm.h>

// Need to lock ALL of the sender enclave's metadata region, the recipient enclave's metadata region (if different)

api_result_t sm_internal_mail_send (enclave_id_t recipient, mailbox_id_t mailbox_id, uintptr_t in_message) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - recipient must be either OWNER_UNTRUSTED or valid enclave
    - if the recipient is an enclave, it must be initialized
    - mailbox_id must be valid
    - the recipient mailbox must be expecting this sender
    - the recipient mailbox must be empty
  */

  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;
  mailbox_t * mailbox;

  region_map_t locked_regions = (const region_map_t){ 0 };
  bool untrusted_locked = false;

  // <TRANSACTION>

  // recipient must be either OWNER_UNTRUSTED or valid enclave
  // Lock the recipient's regions
  if (recipient == OWNER_UNTRUSTED) {

    // mailbox_id must be valid
    if (mailbox_id >= NUM_UNTRUSTED_MAILBOXES) {
      return MONITOR_INVALID_VALUE;
    }

    if (!lock_untrusted_state()) {
      return MONITOR_CONCURRENT_CALL;
    }
    untrusted_locked = true;

    mailbox = &sm->untrusted_mailboxes[mailbox_id];

  } else { // an enclave
    api_result_t result = add_lock_region_iff_valid_enclave(recipient, &locked_regions);
    if ( MONITOR_OK != result ) {
      return result;
    }

    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(recipient);

    // mailbox_id must be valid
    if (mailbox_id >= enclave_metadata->num_mailboxes) {
      unlock_regions(&locked_regions);
      return MONITOR_INVALID_VALUE;
    }

    // if the recipient is an enclave, it must be initialized
    if(enclave_metadata->init_state != ENCLAVE_STATE_INITIALIZED) {
      unlock_regions(&locked_regions);
      return MONITOR_INVALID_STATE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];

  }

  // Lock the caller's region
  if((caller != recipient) && (addr_to_region_id(caller) != addr_to_region_id(recipient))) {

    if(caller == OWNER_UNTRUSTED) {
      if (!lock_untrusted_state()) {
        // Unlock recipient's region (the recipient is an enclave)
        unlock_regions(&locked_regions);

        return MONITOR_CONCURRENT_CALL;
      }
      untrusted_locked = true;
    }
    else if (!add_lock_region(addr_to_region_id(caller), &locked_regions)) {
      // Unlock recipient's region
      if (untrusted_locked) {
        unlock_untrusted_state();
      }
      unlock_regions(&locked_regions);

      return MONITOR_CONCURRENT_CALL;
    }
  }

  // the recipient mailbox must be expecting this sender
  // the recipient mailbox must be empty

  if ((mailbox->expected_sender != caller) || (mailbox->state != ENCLAVE_MAILBOX_STATE_EMPTY)) {
    if (untrusted_locked) {
      unlock_untrusted_state();
    }
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
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
  memcpy_u2m(&mailbox->message, (void *) in_message, sizeof(uint8_t) * MAILBOX_SIZE);

  // Update the mailbox's state
  mailbox->state = ENCLAVE_MAILBOX_STATE_FULL;

  // Release locks
  if (untrusted_locked) {
    unlock_untrusted_state();
  }
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  return MONITOR_OK;
}
