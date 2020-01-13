#include <sm.h>

#error not implemented

TODO: check when the block clock logic is validated - should be at region assign
Lock the calling core
Set block clock for *this* core
SFENCE

api_result_t sm_region_flush () {
  TODO

  return monitor_ok;
}
