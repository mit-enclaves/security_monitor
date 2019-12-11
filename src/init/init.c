#include <stdbool.h>
#include <sm.h>

extern void * payload_ptr;

#ifndef SIGNING_ENCLAVE_MEASUREMENT
  #define SIGNING_ENCLAVE_MEASUREMENT {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

void sm_init(void) {
  // INIT CORES
  // TODO: If this is not core 0 wait for an IPI

  // Initialize core metadata
  for ( int i=0; i<NUM_CORES; i++ ) {
    sm_state->cores[i].owner = OWNER_UNTRUSTED;
    sm_state->cores[i].thread = ENCLAVE_THRE AD_NONE;
    platform_lock_release( &sm_state->cores[i].lock );
  }

  // Initialize region metadata : untrusted SW owns all regions.
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    sm_state->regions[i].owner = OWNER_UNTRUSTED;
    sm_state->regions[i].type = REGION_TYPE_UNTRUSTED;
    sm_state->regions[i].state = REGION_STATE_OWNED;
    platform_lock_release( &sm_state->regions[i].lock );
  }

  TODO: mark any before the end of the SM as of type REGION_TYPE_SM, making them impossible to free and re-allocate

  // Initialize untrusted metadata : untrusted SW is allowed access to all regions.
  for ( int i=0; i<NUM_REGIONS; i++ ) {
    sm_state->untrusted_regions[i] = true;
  }

  // Initialize signing enclave measurement
  const uint8_t signing_enclave_measurement[64] = SIGNING_ENCLAVE_MEASUREMENT;
  memcpy( sm_state->signing_enclave_measurement, signing_enclave_measurement, sizeof(signing_enclave_measurement) );

  // TODO: Send IPI to other core to wake them up

  platform_boot_os( payload_ptr );
}
