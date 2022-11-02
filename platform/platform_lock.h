#ifndef PLATFORM_LOCK_H
#define PLATFORM_LOCK_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint64_t lock_flag;
  uint64_t _pad[7];
} platform_lock_t;

#define platform_lock_acquire(lock) ({ unsigned long __tmp; \
      asm volatile ("amoswap.w.aq %[result], %[value], (%[address]) \n": [result] "=r"(__tmp) : [value] "r"(1), [address] "r"(&((lock)->lock_flag))); \
      (__tmp == 0); })

#define platform_lock_release(lock) ({ \
      asm volatile ("amoswap.w.rl x0, x0, (%[address]) \n" :: [address] "r"(&((lock)->lock_flag))); })

static inline bool platform_lock_state(platform_lock_t *lock) {
  return ((lock->lock_flag) != 0);
}

#endif // PLATFORM_LOCK_H
