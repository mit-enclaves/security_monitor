#include <stdbool.h>
#include <sm.h>

extern void * payload_ptr;

#ifndef SIGNING_ENCLAVE_MEASUREMENT
  #define SIGNING_ENCLAVE_MEASUREMENT {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

void sm_init(void) {
  // INIT CORES
  // TODO: If this is not core 0 wait for an IPI

  // Initialize globals data structure
  sm_state->sm.
  sm_state->os.

  // Initialize signing enclave measurement
  const uint8_t signing_enclave_measurement[64] = SIGNING_ENCLAVE_MEASUREMENT;
  memcpy( sm_state->signing_enclave_measurement, signing_enclave_measurement, sizeof(signing_enclave_measurement) );

  // Initialize cores
  for(int i=0; i<NUM_CORES; i++) {
    sm_state->cores[i].owner_eid = EID_OS;
    sm_state->cores[i].owner_thread = THREAD_OS;
    sm_state->cores[i].is_enclaved = false;
    platform_lock_release( sm_state->cores[i].lock );
  }

  // Initialize regions
  for(int i=0; i<NUM_REGIONS; i++) {
    sm_state->regions[i].type = REGION_TYPE_UNTRUSTED;
    sm_state->regions[i].owner = EID_OS;
    sm_state->regions[i].state = REGION_STATE_OWNED;
    platform_lock_release( sm_state->regions[i].lock );
  }

  // TODO: Send IPI to other core to wake them up

  platform_boot_os( payload_ptr );
}
