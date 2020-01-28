#include <sm.h>

uint64_t sm_internal_thread_metadata_pages () {
  return thread_metadata_pages();
}
