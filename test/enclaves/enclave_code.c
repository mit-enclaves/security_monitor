#include <api_enclave.h>

void enclave_entry() {
  uint64_t result = 45;
  result += 10;

  while(1) {
    result+=10;
  }

  sm_exit_enclave();
}
