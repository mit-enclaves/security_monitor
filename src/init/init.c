#include <stdbool.h>
#include <sm.h>

extern void * payload_ptr;

#ifndef SIGNING_ENCLAVE_MEASUREMENT
  #define SIGNING_ENCLAVE_MEASUREMENT {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

void sm_init(void) {
  // INIT CORES
  // TODO: If this is not core 0 wait for an IPI

  sm_state_t * sm = get_sm_state_ptr();

  // Initialize core metadata
  for ( int i=0; i<NUM_CORES; i++ ) {
    sm->cores[i].owner = OWNER_UNTRUSTED;
    sm->cores[i].thread = ENCLAVE_THREAD_NONE;
    unlock_core(i); // Ensure cores aren't locked
  }

  // Initialize region metadata : untrusted SW owns all regions that do not include SM code.
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    bool region_includes_sm = (uint64_t)region_id_to_addr(i) > (SM_ADDR+SM_LEN);
    sm->regions[i].owner = region_includes_sm ? OWNER_UNTRUSTED : OWNER_SM;
    sm->regions[i].type = REGION_TYPE_UNTRUSTED;
    sm->regions[i].state = REGION_STATE_OWNED;
    unlock_region(i); // Ensure cores aren't locked. the SM must be initialized in a vaccum, with only one thread running, so this is not dangerous.
  }

  // Initialize signing enclave measurement
  const uint8_t signing_enclave_measurement[64] = SIGNING_ENCLAVE_MEASUREMENT;
  memcpy( sm->signing_enclave_measurement.bytes, signing_enclave_measurement, sizeof(signing_enclave_measurement) );

  // Initialize untrusted metadata : untrusted SW is allowed access to all regions.
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    sm->untrusted_regions.flags[i] = true;
  }

  // Initialize untrusted mailboxes : all are empty and unused.
  for ( int i=0; i<NUM_UNTRUSTED_MAILBOXES; i++ ) {
    sm->untrusted_mailboxes[i].state = ENCLAVE_MAILBOX_STATE_UNUSED;
  }

  // Unlock the SM untrusted state
  unlock_untrusted_state();

  // TODO: Send IPI to other core to wake them up

  // payload must set its own stack pointer.
  platform_jump_to_untrusted( &sm->untrusted_regions, UNTRUSTED_ENTRY, 0 );
}
