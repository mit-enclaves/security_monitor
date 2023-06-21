#include <sm.h>

// Need to lock ALL of the caller's metadata region, the message buffer and sender_measurement regions (if different)

api_result_t sm_internal_mail_receive (mailbox_id_t mailbox_id, uintptr_t out_message, uintptr_t out_sender_measurement) {

  /* Caller is authenticated and authorized by the trap routing logic :
   the trap handler and MCAUSE unambiguously identify the caller,
   and the trap handler does not route unauthorized API calls.
  */

  // Validate inputs
  // ---------------

  /*
    - mailbox_id must be valid
    - the recipient mailbox must be full
  */

  region_map_t locked_regions = (const region_map_t){ 0 };
  bool untrusted_locked = false;

  sm_state_t * sm = get_sm_state_ptr();
  enclave_id_t caller = sm->cores[platform_get_core_id()].owner;
  mailbox_t * mailbox;

  // <TRANSACTION>

  // caller must be either OWNER_UNTRUSTED or valid enclave
  // Lock the caller's regions
  if (caller == OWNER_UNTRUSTED) {

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

    if (!add_lock_region(addr_to_region_id(caller), &locked_regions)) {
      return MONITOR_CONCURRENT_CALL;
    }

    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);

    // mailbox_id must be valid
    if (mailbox_id >= enclave_metadata->num_mailboxes) {
      unlock_regions(&locked_regions);
      return MONITOR_INVALID_VALUE;
    }

    mailbox = &enclave_metadata->mailboxes[mailbox_id];

  }

  // the recipient mailbox must be full

  if (mailbox->state != ENCLAVE_MAILBOX_STATE_FULL) {
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
  memcpy_m2u((void *) out_sender_measurement, &mailbox->sender_measurement, sizeof(hash_t));

  // Copy the message
  memcpy_m2u((void *) out_message, &mailbox->message, sizeof(uint8_t) * MAILBOX_SIZE);

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
