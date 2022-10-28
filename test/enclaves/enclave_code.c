#include <api_enclave.h>
#include <libnacl.h>

#define SHARED_MEM_REG (0x8a000000)

void enclave_entry() {
  int *magic = (int *) SHARED_MEM_REG;
  
  while(*magic != 1234);
  
  *magic = 4321;

  sm_exit_enclave();
}
