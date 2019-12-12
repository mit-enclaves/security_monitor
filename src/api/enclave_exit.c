#include <sm.h>

TODO 

void sm_enclave_exit () {

  // Lock caller's metadata region

  // Throw away current context

  // Restore untrusted CPU context
  TODO

  // Unlock thread
  TODO

  // Resume untrusted context
  TODO

  return; // unreachable
}
