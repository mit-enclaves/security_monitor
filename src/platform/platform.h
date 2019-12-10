#ifndef SM_PLATFORM_H
#define SM_PLATFORM_H

#include <sha512.h>

// Lock
// ----

typedef struct {
  uint64_t lock;
  uint64_t _pad[7];
} platform_lock_t;

#define platform_lock_acquire(lock) ({ unsigned long __tmp; \
      asm volatile ("amoswap.w.aq %[result], %[value], (%[address])": [result] "=r"(__tmp) : [value] "r"(1), [address] "r"(&(lock.flag))); \
      ~__tmp; })

#define platform_lock_release(lock) ({ \
      asm volatile ("amoswap.w.rl x0, x0, (%[address])":: [address] "r"(&(lock.flag))); })

// Core context state
// ------------------
typedef uint64_t core_state_t[31];
void platform_save_core( core_state_t * core_state );
void platform_load_core( const core_state_t * core_state );
void platform_clean_core(void);

// Hash function
// -------------

typedef sha512_context hash_context_t;
typedef uint8_t[64] hash_t;

static inline void hash_init( hash_context_t* context ) {
  int sha512_init( context );
}

static inline void hash_extend( hash_context_t * context, const uint8_t * message, size_t message_size ) {
  sha512_update( context, message, message_size );
}

static inline void hash_finalize( hash_context_t* in_context, hash_t out_hash ) {
  sha512_final( in_context, out_hash );
}

// Platform control
// ----------------

void platform_panic (uint64_t error_code) __attribute__((noreturn));
void platform_purge_core (void);
void platform_delegate_to_untrusted ( uint64_t virtual_pc, uint64_t  );
void platform_jump_to_untrusted ( region_map_t * region_map, uint64_t virtual_pc, uint64_t virtual_sp );
void platform_jump_to_enclave ( enclave_id_t enclave_id, e_uint64_t virtual_pc, uint64_t  );

#endif // SM_PLATFORM_H
