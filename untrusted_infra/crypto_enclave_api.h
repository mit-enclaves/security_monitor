#ifndef CRYPTO_ENCLAVE_API_H
#define CRYPTO_ENCLAVE_API_H

#include <stdint.h>
#include <stddef.h>
#include "crypto_enclave/crypto_enclave_util.h"

void hash(const void * in_data,
  size_t in_data_size,
  hash_t * out_hash);

void create_secret_signing_key (
  const key_seed_t * in_seed,
  secret_key_t * out_secret_key);

void compute_public_signing_key (
  const secret_key_t * in_secret_key,
  public_key_t * out_public_key);

void sign (
  const void * in_message,
  const size_t in_message_size,
  const public_key_t * in_public_key,
  const secret_key_t * in_secret_key,
  signature_t * out_signature);

void verify (
  const signature_t * in_signature,
  const void * in_message,
  const size_t in_message_size,
  const public_key_t * in_public_key);

void perform_key_agreement (
  const public_key_t * public_key_A,
  const secret_key_t * secret_key_B,
  symmetric_key_t * out_key);

#endif // CRYPTO_ENCLAVE_API_H
