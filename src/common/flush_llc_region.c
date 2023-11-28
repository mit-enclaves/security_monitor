#include <sm.h>

void flush_llc_region(int region_id){
    sm_state_t *sm = get_sm_state_ptr();

    uintptr_t start_addr = (uintptr_t) region_id_to_addr(region_id);
    int size_index_range = 1 << sm->llc_partitions.lgsizes[region_id];

    for(int stride = 0; stride < LLC_NUM_WAYS; stride++) {
        for(int i = 0; i < size_index_range; i++) {
            uintptr_t index = (size_index_range * stride) + i;
            uintptr_t addr = ZERO_DEVICE_OFFSET | start_addr | (index << LLC_INDEX_OFFSET);
            register uintptr_t t0 asm ("t0") = addr;
            // Activate and deactivate L1 use near the load responsible for the flush.
            asm volatile(" \
            csrsi 0x7ca, 16; \n \
            ld t0, 0(t0); \n \
            csrci 0x7ca, 16; \n\
            " : : "r" (t0));
        }
        asm volatile ("fence" ::: "memory");
    }
}