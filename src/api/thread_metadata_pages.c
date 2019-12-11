#include <sm.h>

uint64_t sm_thread_metadata_pages () {
  // Round up at page granularity
  return ( sizeof(thread_metadata_t)+(PAGE_SIZE-1) ) / PAGE_SIZE;
}
