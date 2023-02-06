#include <crypto_enclave_api.h>
#include <os_util.h>
#include <msgq.h>

void hash(const void * in_data,
    size_t in_data_size,
    hash_t * out_hash) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_HASH;
  msg->args[0] = (uintptr_t) in_data;
  msg->args[1] = (uintptr_t) out_hash;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void create_signing_key_pair (
    const key_seed_t * in_seed,
    uint64_t * out_key_id) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_CREATE_SIGN_K;
  msg->args[0] = (uintptr_t) in_seed;
  msg->args[1] = (uintptr_t) out_key_id;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void get_public_signing_key (
    const uint64_t in_key_id,
    public_key_t * out_public_key) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_GET_SIGN_PK;
  msg->args[0] = (uintptr_t) in_key_id;
  msg->args[1] = (uintptr_t) out_public_key;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void sign (
    const void * in_message,
    const size_t in_message_size,
    const uint64_t in_key_id,
    signature_t * out_signature) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SIGN;
  msg->args[0] = (uintptr_t) in_message;
  msg->args[1] = (uintptr_t) in_message_size;
  msg->args[2] = (uintptr_t) in_key_id;
  msg->args[3] = (uintptr_t) out_signature;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void verify (
    const signature_t * in_signature,
    const void * in_message,
    const size_t in_message_size,
    const public_key_t * in_public_key) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_VERIFY;
  msg->args[0] = (uintptr_t) in_signature;
  msg->args[1] = (uintptr_t) in_message;
  msg->args[2] = (uintptr_t) in_message_size;
  msg->args[3] = (uintptr_t) in_public_key;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void perform_key_agreement (
    const public_key_t * public_key_A,
    const secret_key_t * secret_key_B,
    symmetric_key_t * out_key) {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_KEY_AGREEMENT;
  msg->args[0] = (uintptr_t) public_key_A;
  msg->args[1] = (uintptr_t) secret_key_B;
  msg->args[2] = (uintptr_t) out_key;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void enclave_exit() {
  queue_t *q = SHARED_REQU_QUEUE;  
  msg_t *msg = (msg_t *) malloc(sizeof(msg_t));
  msg->f = F_EXIT;
  int ret;
  do {
    ret = push(q, msg);
  } while(ret != 0);
}

void init_enclave_queues() {
  queue_t *qrequ = SHARED_REQU_QUEUE;  
  queue_t *qresp = SHARED_RESP_QUEUE;
  init_q(qrequ);
  init_q(qresp);
}

bool req_queue_is_full() {
  queue_t *q = SHARED_REQU_QUEUE;  
  return is_full(q); 
}

bool resp_queue_is_empty() {
  queue_t *q = SHARED_RESP_QUEUE;  
  return is_empty(q); 
}
