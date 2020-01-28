#include <secure_boot/secure_boot.h>

// TODO: initialize this

sm_keys_t sm_keys __attribute__ ((section (".sm.state"))) = {0};

/*
sm_keys_t:
  public_key_t manufacturer_public_key;

  public_key_t device_public_key;
  signature_t device_signature;

  hash_t software_measurement;
  public_key_t software_public_key;
  secret_key_t software_secret_key;
  signature_t software_signature;

  size_t software_measured_bytes;
  uint8_t* software_measured_binary[];
*/
