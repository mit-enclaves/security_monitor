#include <api_enclave.h>
#include "cryptography.h"
#include <msgq.h>
#include <crypto_enclave_util.h>

#define SHARED_MEM_REG (0x8a000000)

void enclave_entry() {
  queue_t * q = (queue_t *) SHARED_MEM_REG;
  msg_t *m;
  int ret;

  while(true) {
    ret = pop(q, (void **) &m);
    if(ret != 0) continue;
    switch((m)->f) {
      case F_ADDITION:
        m->ret = (int) m->args[0] + m->args[1];
        m->done = true;
        break;
      case F_HASH:
        hash((const void *) m->args[0],
            (size_t) m->args[1],
            (hash_t *) m->args[2]);
        m->done = true;
        break;
      case F_CREATE_SIGN_SK:
        create_secret_signing_key(
            (key_seed_t *) m->args[0],
            (secret_key_t *) m->args[1]);
        m->done = true;
        break;
      case F_COMPUTE_SIGN_PK:
        compute_public_signing_key(
            (secret_key_t *) m->args[0],
            (public_key_t *) m->args[1]);
        m->done = true;
        break;
      case F_SIGN:
        sign(
            (const void *) m->args[0],
            (const size_t) m->args[1],
            (const public_key_t *) m->args[2],
            (const secret_key_t *) m->args[3],
            (signature_t *) m->args[4]);
        m->done = true;
        break;
      case F_VERIFY:
        m->ret = verify(
            (signature_t *) m->args[0],
            (const void *) m->args[1],
            (const size_t) m->args[2],
            (const public_key_t *) m->args[4]);
        m->done = true;
        break;
      case F_KEY_AGREEMENT:
        break;
      case F_EXIT:
        sm_exit_enclave();
      default:
        break;
    } 
  }
}
