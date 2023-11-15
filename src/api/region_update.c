#include <sm.h>

api_result_t sm_internal_region_update () {
  sm_state_t * sm = get_sm_state_ptr();
  int core_id = platform_get_core_id();

  // <TRANSACTION>
  if ( !lock_untrusted_state() ) {
    return MONITOR_CONCURRENT_CALL;
  }

  if ( !lock_core(core_id) ) {
    unlock_untrusted_state();
    return MONITOR_CONCURRENT_CALL;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------
  
  platform_update_memory_protection(sm, core_id);

  unlock_core(core_id);
  unlock_untrusted_state();

  return MONITOR_OK;
}