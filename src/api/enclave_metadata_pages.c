#include <sm.h>

uint64_t sm_internal_enclave_metadata_pages ( uint64_t num_mailboxes ) {
  return enclave_metadata_pages(num_mailboxes);
}
