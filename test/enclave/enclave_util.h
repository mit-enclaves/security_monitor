#ifndef ENCLAVE_UTIL_H
#define ENCLAVE_UTIL_H

#define E_CODE __attribute__((section(".text.enclave_code")))

// UBI CALL MACRO

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

#endif // ENCLAVE_UTIL_H
