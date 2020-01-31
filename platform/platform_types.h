#ifndef SM_PLATFORM_TYPES_H
#define SM_PLATFORM_TYPES_H

#include <stdint.h>

typedef struct {
  uint64_t lock_flag;
  uint64_t _pad[7];
} platform_lock_t;

typedef uint64_t platform_core_state_t[32];

typedef struct {
  uintptr_t ev_base;
  uintptr_t ev_mask;

  uintptr_t meparbase;
  uintptr_t meparmask;
  
  uintptr_t eptbr;
} platform_csr_t;

#endif // SM_PLATFORM_TYPES_H
