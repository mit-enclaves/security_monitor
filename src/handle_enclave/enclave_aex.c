#include <sm.h>

void enclave_aex () {
  // TODO: Save context and signal AEX has occured
  // TODO: Include an API to check for presence of AEX
  // TODO: Include an API to restore API context, or copy it into the calling thread.

  enclave_exit();
}
