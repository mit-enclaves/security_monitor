#include <sm.h>
#include <sm_types.h>
#include <stdint.h>
#include "handle_enclave/handles_enclave.h"

api_result_t sm_internal_random (uintptr_t random_buf, size_t num_bytes) {
    // Check that hash points into a DRAM region owned by the os
    /*
      - random_buf must be either OWNER_UNTRUSTED or valid enclave
      - the buffer must fit entirely in one page
    */

    if (num_bytes > 4096)
        return MONITOR_INVALID_STATE;

    sm_state_t * sm = get_sm_state_ptr();
    enclave_id_t caller = sm->cores[platform_get_core_id()].owner;

    enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(caller);
    region_map_t regions = enclave_metadata->regions;
    uintptr_t phys_base = (uintptr_t)RAM_BASE;
    for (int i = 0; i < sizeof(regions.flags); i++) {
      phys_base +=  regions.flags[i] * (i << REGION_SHIFT);
    }

    random_buf = phys_base + random_buf + 0x8000; /* enclave overhead */

    if (region_owner(addr_to_region_id(random_buf)) != caller)
        return MONITOR_INVALID_STATE;

    uint64_t *random_hw = (uint64_t *)0x51000000;
    for (int i = 0; i < num_bytes; i += sizeof(uint64_t)) {
        uint64_t result = *random_hw;
        memcpy((void *)(random_buf + i), &result, sizeof(result));
    }

    return MONITOR_OK;
}
