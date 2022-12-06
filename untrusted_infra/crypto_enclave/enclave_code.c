#include <api_enclave.h>
#include "cryptography.h"
#include <msgq.h>
#include <crypto_enclave_util.h>

#define SHARED_MEM_REG (0x8a000000)
#define SHARED_REQU_QUEUE ((queue_t *) SHARED_MEM_REG)
#define SHARED_RESP_QUEUE ((queue_t *) (SHARED_MEM_REG + sizeof(queue_t)))

#define riscv_perf_cntr_begin() asm volatile("csrwi 0x801, 1")
#define riscv_perf_cntr_end() asm volatile("csrwi 0x801, 0")


void enclave_entry() {
  queue_t * qreq = SHARED_REQU_QUEUE;
  queue_t * qres = SHARED_RESP_QUEUE;
  
  msg_t *m;
  int ret;
    
  // *** BEGINING BENCHMARK ***
  //riscv_perf_cntr_begin();

  while(true) {
    ret = pop(qreq, (void **) &m);
    if(ret != 0) continue;
    switch((m)->f) {
      case F_ADDITION:
        m->ret = (int) m->args[0] + m->args[1];
        break;
      case F_HASH:
        hash((const void *) m->args[0],
            (size_t) m->args[1],
            (hash_t *) m->args[2]);
        break;
      case F_CREATE_SIGN_SK:
        create_secret_signing_key(
            (key_seed_t *) m->args[0],
            (secret_key_t *) m->args[1]);
        break;
      case F_COMPUTE_SIGN_PK:
        compute_public_signing_key(
            (secret_key_t *) m->args[0],
            (public_key_t *) m->args[1]);
        break;
      case F_SIGN:
        sign(
            (const void *) m->args[0],
            (const size_t) m->args[1],
            (const public_key_t *) m->args[2],
            (const secret_key_t *) m->args[3],
            (signature_t *) m->args[4]);
        break;
      case F_VERIFY:
        m->ret = verify(
            (signature_t *) m->args[0],
            (const void *) m->args[1],
            (const size_t) m->args[2],
            (const public_key_t *) m->args[3]);
        break;
      case F_KEY_AGREEMENT:
        break;
      case F_EXIT:
        m->done = true;
        do {
          ret = push(qres, m);
        } while(ret != 0);
	//riscv_perf_cntr_end();
	// *** END BENCHMARK *** 
        sm_exit_enclave();
      default:
        break;
    } 
    m->done = true;
    do {
      ret = push(qres, m);
    } while(ret != 0);
  }
}
