#include <sm.h>

void flush_llc_region(int region_id){
    sm_state_t *sm = get_sm_state_ptr();

    uintptr_t start_addr = (uintptr_t) region_id_to_addr(region_id);
    int size_index_range = 1 << sm->llc_partitions.lgsizes[region_id];

    volatile uint64_t buff = 0;
    for(int stride = 0; stride < LLC_NUM_WAYS; stride++) {
        for(int i = 0; i < size_index_range; i++) {
            uintptr_t index = (size_index_range * stride) + i;
            uintptr_t addr = ZERO_DEVICE_OFFSET | start_addr | (index << LLC_INDEX_OFFSET);
            printm("Flush addr %llx\n", addr);
            buff = *((uint64_t *) addr);
        }
        asm volatile ("fence" ::: "memory");
    }
}