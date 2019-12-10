#include <sm.h>

uint64_t sm_enclave_metadata_pages (uint64_t mailbox_count) {
  // Round up at page granularity
  size_t enclave_size = sizeof(sm_enclave_t) + mailbox_count*sizeof(mailbox_t);
  return ( enclave_size+(PAGE_SIZE-1) ) / PAGE_SIZE;
}
