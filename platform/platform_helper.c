#include <sm.h>

uint64_t regions_to_bitmap(region_map_t *regions) {
  uint64_t bitmap =0x0;
  for(int i = 0; i < NUM_REGIONS; i++) {
    uint64_t mask = 1ul << i;
    if(regions->flags[i]) {
      bitmap |= mask;
    }
  }
  return bitmap;
}

region_map_t bitmap_to_regions(uint64_t bitmap) {
  region_map_t ret;
  for(int i = 0; i < NUM_REGIONS; i++) {
    ret.flags[i] = region_is_accessible(bitmap, i);
  }
  return ret;
}