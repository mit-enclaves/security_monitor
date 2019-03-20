#include <api.h>
#include <data_structures.h>

dram_region_state_t dram_region_state(size_t dram_region) {
	return ((dram_region_t) dram_region).type; // TODO do you need to try to grab the lock? YES
}

enclave_id_t dram_region_owner(size_t dram_region) {
	return ((dram_region_t) dram_region).owner;
}

api_result_t assign_dram_region(size_t dram_region, enclave_id_t new_owner);

api_result_t free_dram_region(size_t dram_region);

api_result_t s
