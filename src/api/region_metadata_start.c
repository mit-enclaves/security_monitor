#include <sm.h>

uint64_t sm_internal_region_metadata_start () {
  return get_metadata_start_page();
}
