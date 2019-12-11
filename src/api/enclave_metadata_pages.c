#include <sm.h>

uint64_t sm_enclave_metadata_pages ( uint64_t num_mailboxes ) {
  // Round up at page granularity
  size_t enclave_size = sizeof(enclave_metadata_t) + num_mailboxes*sizeof(mailbox_t);
  return ( enclave_size+(PAGE_SIZE-1) ) / PAGE_SIZE;
}
