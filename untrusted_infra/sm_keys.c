#include <stdint.h>
#include <stddef.h>

typedef struct hash_t {
  uint8_t bytes[64];
} hash_t;

typedef struct public_key_t {
  uint8_t bytes[32];
} public_key_t;

typedef struct secret_key_t {
  uint8_t bytes[64];
} secret_key_t;

typedef struct symmetric_key_t {
  uint8_t bytes[64];
} symmetric_key_t; 

typedef struct signature_t {
  uint8_t bytes[64];
} signature_t;

typedef struct boot_image_header_t {
  public_key_t manufacturer_public_key;

  uint32_t device_public_key_present;
  public_key_t device_public_key;
  signature_t device_signature;

  uint32_t software_public_key_present;
  hash_t software_measurement;
  public_key_t software_public_key;
  secret_key_t software_secret_key;
  signature_t software_signature;

  size_t software_measured_bytes;
  uint8_t* software_measured_binary[];
} boot_image_header_t;

// TODO: initialize this

boot_image_header_t sm_keys __attribute__ ((section (".sm.state"))) = {0};
