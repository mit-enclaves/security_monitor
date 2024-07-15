#include <api_enclave.h>

#define SHARED_MEM_REG (0x8a000000)

#if (DEBUG_ENCLAVE == 1)
#include "../sbi/console.h"
#endif

bool first_entrance = true;

#define MAILBOX_SIZE  (0x100)

void enclave_entry() {

#if (DEBUG_ENCLAVE == 1)
  console_init();
#endif
  
  if(first_entrance) {
#if (DEBUG_ENCLAVE == 1)
    printm("Hi from inside!\n");
#endif
    hash_t m;
    public_key_t pk;
    secret_key_t sk;
    signature_t s;
    
    public_key_t pk_d;
    public_key_t pk_sm;
    hash_t h_sm;
    signature_t s_sm;
    
    sm_enclave_get_keys(&m, &pk, &sk, &s);
    sm_get_public_field(PUBLIC_FIELD_PK_D, &pk_d);
    sm_get_public_field(PUBLIC_FIELD_PK_SM, &pk_sm);
    sm_get_public_field(PUBLIC_FIELD_H_SM, &h_sm);
    sm_get_public_field(PUBLIC_FIELD_SIG_SM, &s_sm);

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
    
    printm("PK_D: [");
    for(int i = 0; i < sizeof(public_key_t); i++) {
      printm("%x, ", pk_d.bytes[i]);
    }
    printm("]\n");
    
    printm("PK_SM: [");
    for(int i = 0; i < sizeof(public_key_t); i++) {
      printm("%x, ", pk_sm.bytes[i]);
    }
    printm("]\n");
    
    printm("H_SM: [");
    for(int i = 0; i < sizeof(hash_t); i++) {
      printm("%x, ", h_sm.bytes[i]);
    }
    printm("]\n");
    
    printm("S_SM: [");
    for(int i = 0; i < sizeof(signature_t); i++) {
      printm("%x, ", s_sm.bytes[i]);
    }
    printm("]\n");
#endif

    sm_mail_accept(0, OWNER_UNTRUSTED);
    first_entrance = false;
  }
  else {
    char msg[MAILBOX_SIZE];
    hash_t exp_sndr;
    sm_mail_receive(0, &msg, &exp_sndr);

#if (DEBUG_ENCLAVE == 1)
    printm("I've been here before!\n");
    printm("I received: \n\"%s\"\n", msg);
#endif
    
    char* msg_out = "Hello OS!";
    sm_mail_send(OWNER_UNTRUSTED, 0, msg_out);

    sm_exit_enclave();
  }
  
  sm_exit_enclave();
}
