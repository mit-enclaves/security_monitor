#include <api_enclave.h>
#include <libnacl.h>
#include <msgq.h>
#include <crypto_enclave_api.h>

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
        (m)->args[0] += (m)->args[1];
        break;
      case F_EXIT:
        sm_exit_enclave();
      default:
        break;
    } 
  }
}
