#ifndef SM_PLATFORM_H
#define SM_PLATFORM_H

#include "platform_types.h"
#include <sm_types.h>

// Lock
// ----
#define platform_lock_acquire(lock) ({ unsigned long __tmp; \
      asm volatile ("amoswap.w.aq %[result], %[value], (%[address]) \n": [result] "=r"(__tmp) : [value] "r"(1), [address] "r"(&((lock)->lock_flag))); \
      ~__tmp; })

#define platform_lock_release(lock) ({ \
      asm volatile ("amoswap.w.rl x0, x0, (%[address]) \n":: [address] "r"(&((lock)->lock_flag))); })


// Core context state
// ------------------
void platform_save_core( platform_core_state_t * core_state );
void platform_load_core( const platform_core_state_t * core_state );
void platform_clean_core(void);


// Platform control
// ----------------
void platform_panic (uint64_t error_code) __attribute__((noreturn));
void platform_purge_core (void);
void platform_delegate_to_untrusted ( uint64_t virtual_pc, uint64_t  );
void platform_jump_to_untrusted ( region_map_t * region_map, uint64_t virtual_pc, uint64_t virtual_sp );
void platform_jump_to_enclave ( enclave_id_t enclave_id, uint64_t virtual_pc, uint64_t  );

#endif // SM_PLATFORM_H
