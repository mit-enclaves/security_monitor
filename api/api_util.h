#ifndef API_UTIL_H
#define API_UTIL_H

#include "api_types.h"

#define SM_API_CALL(api_id, arg0, arg1, arg2, arg3, arg4, arg5, arg6) ({ \
    register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0); \
    register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1); \
    register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2); \
    register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3); \
    register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4); \
    register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5); \
    register uintptr_t a6 asm ("a6") = (uintptr_t)(arg6); \
    register uintptr_t a7 asm ("a7") = (uintptr_t)(api_id); \
    asm volatile( "ecall" \
                : "+r" (a0) \
                : "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7) \
                : "memory" ); \
    a0; \
  })

#endif // API_UTIL_H
