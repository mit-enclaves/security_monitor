#include <api_enclave.h>

#define SHARED_MEM_REG (0x8a000000)

#if (DEBUG_ENCLAVE == 1)
#include "../sbi/console.h"
#endif

void enclave_entry() {
#if (DEBUG_ENCLAVE == 1)
  printm("Hi from inside!\n");
#endif
  int a = 0;
  a += 90;
  sm_exit_enclave();
}
