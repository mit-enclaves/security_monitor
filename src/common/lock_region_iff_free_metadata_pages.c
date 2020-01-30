#include <sm.h>

api_result_t add_lock_region_iff_free_metadata_pages( uintptr_t ptr, uint64_t num_pages, region_map_t * locked_regions) {
  // Get a handler to SM global state
  sm_state_t * sm = get_sm_state_ptr();

  // Check that ptr is page alligned
  if ( !is_page_aligned(ptr) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Check that the requested region is indeed a region in RAM
  uint64_t region_id = addr_to_region_id(ptr);
  if ( !is_valid_region_id(region_id) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Check that the chosen page does not overlap with region page info structure.
  uint64_t page_id = addr_to_region_page_id( ptr );
  // region page info table is marked as METADATA_INVALID, so we do not need to explicitly check that page_id > sm_region_metadata_start().

  // Check that the requested pages do not span regions
  if ( !is_valid_page_id_in_region(page_id+num_pages-1) ) {
    return MONITOR_INVALID_VALUE;
  }

  // Compute if the region was already locked
  bool not_previously_locked = locked_regions->flags[region_id];

  // Begin transation
  // ----------------

  if (!add_lock_region(region_id, locked_regions)) {
    return MONITOR_CONCURRENT_CALL;
  }

  // Check that the requested region is indeed a metadata region
  if ( sm->regions[region_id].type != REGION_TYPE_METADATA ) {
    if(not_previously_locked) {
      unlock_region(region_id);
    }
    return MONITOR_INVALID_STATE;
  }

  // Check that the requested region is owned (not blocked)
  if ( sm->regions[region_id].state != REGION_STATE_OWNED ) {
    if(not_previously_locked) {
      unlock_region(region_id);
    }
    return MONITOR_INVALID_STATE;
  }

  // Check that each of the requested metadata pages is of type METADATA_PAGE_FREE
  for ( int i=0; i<num_pages; i++ ) {
    metadata_region_t * metadata_region = region_id_to_addr(region_id);
    if ( metadata_region->page_info[page_id+i] != METADATA_PAGE_FREE ) {
      if(not_previously_locked) {
        unlock_region(region_id);
      }
      return MONITOR_INVALID_STATE;
    }
  }

  // All seems well; the region is now locked
  return MONITOR_OK;
}
