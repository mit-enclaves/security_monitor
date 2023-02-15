#include <api_enclave.h>

#define SHARED_MEM_REG (0x8a000000)

#if (DEBUG_ENCLAVE == 1)
#include "../sbi/console.h"
#endif

typedef struct hash_t {
  uint8_t bytes[64];
} hash_t;

#define LENGTH_SEED 32
#define LENGTH_PK 32
#define LENGTH_SK 64
#define LENGTH_S 64

typedef struct public_key_t {
  uint8_t bytes[LENGTH_PK];
} public_key_t;

typedef struct secret_key_t {
  uint8_t bytes[LENGTH_SK];
} secret_key_t;

typedef struct signature_t {
  uint8_t bytes[LENGTH_S];
} signature_t;


void enclave_entry() {
#if (DEBUG_ENCLAVE == 1)
  printm("Hi from inside!\n");
#endif
  hash_t m;
  public_key_t pk;
  secret_key_t sk;
  signature_t s;
  
  sm_enclave_get_keys(&m, &pk, &sk, &s);

#if (DEBUG_ENCLAVE == 1)
  printm("Enclave_measurement : [");
  for(int i = 0; i < sizeof(hash_t); i++) {
    printm("%x, ", m.bytes[i]);
  }
  printm("]\n");
  
  printm("PK: [");
  for(int i = 0; i < sizeof(public_key_t); i++) {
    printm("%x, ", pk.bytes[i]);
  }
  printm("]\n");
  
  printm("SK: [");
  for(int i = 0; i < sizeof(secret_key_t); i++) {
    printm("%x, ", sk.bytes[i]);
  }
  printm("]\n");
  
  printm("S: [");
  for(int i = 0; i < sizeof(signature_t); i++) {
    printm("%x, ", s.bytes[i]);
  }
  printm("]\n");
#endif
  
  sm_exit_enclave();
}
