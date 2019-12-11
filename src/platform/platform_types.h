#ifndef SM_PLATFORM_TYPES_H
#define SM_PLATFORM_TYPES_H

#include <stdint.h>

typedef struct {
  uint64_t lock_flag;
  uint64_t _pad[7];
} platform_lock_t;

typedef uint64_t platform_core_state_t[32];

#endif // SM_PLATFORM_TYPES_H
