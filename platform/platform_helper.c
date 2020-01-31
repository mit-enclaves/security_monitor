#include <sm.h>

uint64_t regions_to_bitmap(region_map_t *regions) {
  uint64_t bitmap =0x0;
  for(int i = 0; i < NUM_REGIONS; i++) {
    bitmap = bitmap << 1;
    bitmap += regions->flags[i] ? 1 : 0;
  }
  return bitmap;
}
