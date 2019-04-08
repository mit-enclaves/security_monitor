#ifndef SM_UTIL_H
#define SM_UTIL_H

#include <data_structures.h>

#define REGION_IDX(addr) ((1u<<(addr >> intlog2(SIZE_DRAM / NUM_REGIONS))) & (NUM_REGIONS - 1))
#define METADATA_IDX(addr) (((addr % SIZE_REGION) - (sizeof(metadata_mage_map_entry_t) * NUM_METADATA_PAGES_PER_REGION)) / NUM_METADATA_PAGES_PER_REGION)

inline bool is_valid_enclave(enclave_id_t enclave_id);
inline bool owned(uintptr_t phys_addr, enclave_id_t enclave_id);
inline bool check_buffer_ownership(uintptr_t buff_phys_addr, size_t size_buff, enclave_id_t enclave_id);	

#endif // SM_UTIL_H
