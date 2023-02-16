#ifndef CRYPTO_ENCLAVE_UTIL_H
#define CRYPTO_ENCLAVE_UTIL_H

#include <api_crypto_types.h>
#include <stdbool.h>

#define F_ADDITION             0
#define F_HASH                 1
#define F_CREATE_SIGN_K        2
#define F_GET_SIGN_PK          3
#define F_SIGN                 4
#define F_VERIFY               5
#define F_KEY_AGREEMENT        6
#define F_EXIT                 20

#define LENGTH_SEED 32

typedef struct secret_key_seed_t {
  uint8_t bytes[LENGTH_SEED];
} key_seed_t;

typedef struct symmetric_key_t {
  uint8_t bytes[64];
} symmetric_key_t; 

typedef struct msg_t {
  int f;
  uintptr_t args[5];
  int ret;
  bool done;
} msg_t;

typedef struct key_entry_t {
  bool init;
  public_key_t pk;
  secret_key_t sk;
} key_entry_t;

#endif // CRYPTO_ENCLAVE_UTIL_H
