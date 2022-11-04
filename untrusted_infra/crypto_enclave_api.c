#include <crypto_enclave_api.h>
#include <os_util.h>
#include <msgq.h>

void hash(const void * in_data,
    size_t in_data_size,
    hash_t * out_hash) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_HASH;
  msg->args[0] = (uintptr_t) in_data;
  msg->args[1] = (uintptr_t) out_hash;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void create_secret_signing_key (
    const key_seed_t * in_seed,
    secret_key_t * out_secret_key) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_CREATE_SIGN_SK;
  msg->args[0] = (uintptr_t) in_seed;
  msg->args[1] = (uintptr_t) out_secret_key;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void compute_public_signing_key (
    const secret_key_t * in_secret_key,
    public_key_t * out_public_key) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_COMPUTE_SIGN_PK;
  msg->args[0] = (uintptr_t) in_secret_key;
  msg->args[1] = (uintptr_t) out_public_key;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void sign (
    const void * in_message,
    const size_t in_message_size,
    const public_key_t * in_public_key,
    const secret_key_t * in_secret_key,
    signature_t * out_signature) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SIGN;
  msg->args[0] = (uintptr_t) in_message;
  msg->args[1] = (uintptr_t) in_message_size;
  msg->args[2] = (uintptr_t) in_public_key;
  msg->args[3] = (uintptr_t) in_secret_key;
  msg->args[4] = (uintptr_t) out_signature;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void verify (
    const signature_t * in_signature,
    const void * in_message,
    const size_t in_message_size,
    const public_key_t * in_public_key) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_VERIFY;
  msg->args[0] = (uintptr_t) in_signature;
  msg->args[1] = (uintptr_t) in_message;
  msg->args[2] = (uintptr_t) in_message_size;
  msg->args[3] = (uintptr_t) in_public_key;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void perform_key_agreement (
    const public_key_t * public_key_A,
    const secret_key_t * secret_key_B,
    symmetric_key_t * out_key) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_KEY_AGREEMENT;
  msg->args[0] = (uintptr_t) public_key_A;
  msg->args[1] = (uintptr_t) secret_key_B;
  msg->args[2] = (uintptr_t) out_key;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
}

void enclave_exit() {
  queue_t *q = SHARED_QUEUE;  
  msg_t *m = (msg_t *) malloc(sizeof(msg_t));
  m->f = F_EXIT;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, m);
  }
}
