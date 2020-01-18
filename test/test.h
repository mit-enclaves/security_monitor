#ifndef SM_TEST_H
#define SM_TEST_H

#include <api.h>
#include "htif/htif.h"

void test_entry (void)  __attribute__((noreturn));



static inline void test_success (void) __attribute__((noreturn));
static inline void test_success (void) {
  tohost = TOHOST_CMD(0, 0, 0b01);
  while (1) { continue; }
}

// TODO: add test fail helper

#define SM_API_CALL(api_id, arg0, arg1, arg2, arg3, arg4, arg5) ({ \
    register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0); \
    register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1); \
    register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2); \
    register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3); \
    register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4); \
    register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5); \
    register uintptr_t a7 asm ("a7") = (uintptr_t)(api_id); \
    asm volatile( "ecall" \
                : "+r" (a0) \
                : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a7) \
                : "memory" ); \
    a0; \
  })

#endif // SM_TEST_H
