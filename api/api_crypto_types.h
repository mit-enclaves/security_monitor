#ifndef API_CRYPTO_TYPES_H
#define API_CRYPTO_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define LENGTH_HASH 64
#define LENGTH_SEED 32
#define LENGTH_PK 32
#define LENGTH_SK 64
#define LENGTH_SIG 64
#define LENGTH_SYM_KEY 64

typedef struct hash_t {
  uint8_t bytes[LENGTH_HASH];
} hash_t;

typedef struct secret_key_seed_t {
  uint8_t bytes[LENGTH_SEED];
} key_seed_t;

typedef struct public_key_t {
  uint8_t bytes[LENGTH_PK];
} public_key_t;

typedef struct secret_key_t {
  uint8_t bytes[LENGTH_SK];
} secret_key_t;

typedef struct signature_t {
  uint8_t bytes[LENGTH_SIG];
} signature_t;

typedef struct symmetric_key_t {
  uint8_t bytes[LENGTH_SYM_KEY];
} symmetric_key_t; 

#endif // API_CRYPTO_TYPES_H
