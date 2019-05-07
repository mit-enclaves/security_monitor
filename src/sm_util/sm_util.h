#ifndef SM_UTIL_H
#define SM_UTIL_H

#include <data_structures.h>
#include <ecall_s.h>

// SECTION MACROS

#define SM_UTRAP __attribute__((section(".sm.text.untrusted_trap")))

#define SM_ETRAP __attribute__((section(".sm.text.enclave_trap")))

// SBI CALL MACRO

#define SBI_SM_OS_CALL(code, arg0, arg1, arg2, arg3, arg4, arg5) ({ \
      register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);  \
      register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);  \
      register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);  \
      register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3);  \
      register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4);  \
      register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5);  \
      register uintptr_t a7 asm ("a7") = (uintptr_t)(code);  \
      asm volatile ("ecall"                                  \
            : "+r" (a0)                                      \
            : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a7) \
            : "memory");                                     \
      a0;                                                    \
      })

#define UBI_SM_ENCLAVE_CALL(code, arg0, arg1, arg2) ({ \
      register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);  \
      register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);  \
      register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);  \
      register uintptr_t a7 asm ("a7") = (uintptr_t)(code);  \
      asm volatile ("ecall"                                  \
            : "+r" (a0)                                      \
            : "r" (a1), "r" (a2), "r" (a7) \
            : "memory");                                     \
      a0;                                                    \
      })

// CONSTANTS MANAGEMENT

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
