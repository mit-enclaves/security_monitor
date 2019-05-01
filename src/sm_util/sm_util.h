#ifndef SM_UTIL_H
#define SM_UTIL_H

#include <data_structures.h>

static inline int intlog2(int n) {
   int cnt = 0;
   while(n >>= 1) {
      cnt++;
   }
   return cnt;
}

#define SIZE_REGION (SIZE_DRAM / NUM_REGIONS)
#define REGION_IDX(addr) (1ul<<(addr >> intlog2(SIZE_REGION))) 

#define NUM_METADATA_PAGES_PER_REGION (SIZE_REGION/(sizeof(metadata_page_map_entry_t) + SIZE_PAGE))
#define METADATA_IDX(addr) (((addr % SIZE_REGION) - (sizeof(metadata_page_map_entry_t) * NUM_METADATA_PAGES_PER_REGION)) / NUM_METADATA_PAGES_PER_REGION)

bool is_valid_enclave(enclave_id_t enclave_id);
bool owned(uintptr_t phys_addr, enclave_id_t enclave_id);
bool check_buffer_ownership(uintptr_t buff_phys_addr, size_t size_buff, enclave_id_t enclave_id);

api_result_t is_valid_thread(enclave_id_t enclave_id, thread_id_t thread_id);

// PAGE TABLE MANAGEMENT

#define PAGE_OFFSET (intlog2(SIZE_PAGE))
#define PN_MASK ((1ul << PN_OFFSET) - 1)
#define PPN2_MASK ((1ul << PPN2_OFFSET) - 1)
#define ACL_MASK ((1ul << PAGE_ENTRY_ACL_OFFSET) - 1)
#define PPNs_MASK ((PPN2_MASK << (PPN2_OFFSET + (PN_OFFSET * 2))) | (PN_MASK << (PN_OFFSET * 2)) | (PN_MASK << (PN_OFFSET)))

#define PTE_V (1ul)
#define PTE_R (1ul << 1)
#define PTE_W (1ul << 2)
#define PTE_X (1ul << 3)

#define SIZE_ENCLAVE_HANDLER // TODO: DEFINE

#endif // SM_UTIL_H
