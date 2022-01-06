#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

// Hash
#include "ed25519/sha512.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct hash_t {
  uint8_t bytes[64];
} hash_t;

typedef sha512_context hash_context_t;

static inline void hash(const void * in_data,
  size_t in_data_size,
  hash_t * out_hash) {

  // mdlen = hash output in bytes
  // digest goes to md
  sha512(in_data, in_data_size, out_hash->bytes);
}

static inline void hash_init(
  hash_context_t * hash_context) {

  sha512_init(hash_context);    // mdlen = hash output in bytes
}

static inline void hash_extend(
  hash_context_t * hash_context,
  const void * in_data,
  size_t in_data_size) {

  sha512_update(hash_context, in_data, in_data_size);
}

static inline void hash_finalize(
  hash_context_t * hash_context,
  hash_t * out_hash) {

  // digest goes to md
  sha512_final(hash_context, out_hash->bytes);
}

// Signatures

#define ED25519_NO_SEED 1
#include "ed25519/ed25519.h"

#define LENGTH_SEED 32
#define LENGTH_PK 32
#define LENGTH_SK 64

typedef struct private_key_seed_t {
  uint8_t bytes[LENGTH_SEED];
} key_seed_t;

typedef struct public_key_t {
  uint8_t bytes[LENGTH_PK];
} public_key_t;

typedef struct secret_key_t {
  uint8_t bytes[LENGTH_SK];
} secret_key_t;

typedef struct symmetric_key_t {
  uint8_t bytes[64];
} symmetric_key_t; 

typedef struct signature_t {
  uint8_t bytes[64];
} signature_t;


static inline void create_secret_signing_key (
  const key_seed_t * in_seed,
  secret_key_t * out_secret_key) {

  ed25519_create_privkey( out_secret_key->bytes, (uint8_t *)in_seed->bytes );
}

static inline void compute_public_signing_key (
  const secret_key_t * in_secret_key,
  public_key_t * out_public_key) {

  ed25519_compute_pubkey( out_public_key->bytes, (uint8_t *)in_secret_key->bytes );
}

static inline void sign (
  const void * in_message,
  const size_t in_message_size,
  const public_key_t * in_public_key,
  const secret_key_t * in_secret_key,
  signature_t * out_signature) {

  ed25519_sign( out_signature->bytes,
    (uint8_t *)in_message,
    in_message_size,
    (uint8_t *)in_public_key->bytes,
    (uint8_t *)in_secret_key->bytes );
}

static inline bool verify (
  const signature_t * in_signature,
  const void * in_message,
  const size_t in_message_size,
  const public_key_t * in_public_key) {

  return 1 == ed25519_verify( in_signature->bytes,
    in_message,
    in_message_size,
    in_public_key->bytes );
}

// Key agreement
static inline void perform_key_agreement (
  const public_key_t * public_key_A,
  const secret_key_t * secret_key_B,
  symmetric_key_t * out_key) {

  // Agreed-upon value is 32 Bytes, conveniently equal to the symmetric key size
  ed25519_key_exchange( out_key->bytes, public_key_A->bytes, secret_key_B->bytes );
}

#endif
