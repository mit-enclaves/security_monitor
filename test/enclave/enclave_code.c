#include <api_enclave.h>

#define SHARED_MEM_REG (0x8a000000)

void enclave_entry() {
  int a = 0;
  a += 90;
  sm_exit_enclave();
}
