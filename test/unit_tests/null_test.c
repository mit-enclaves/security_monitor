#include <test.h>

void test_entry (void) {


  // Set up an untrusted API call
  uint8_t pk_buffer[32];

  api_result_t result = SM_API_CALL(SM_GET_PUBLIC_FIELD, PUBLIC_FIELD_PK_M, &pk_buffer, 0, 0, 0, 0);

  // Set up a fake enclave API call

  /*
  register uintptr_t a7 asm ("a7") = (uint64_t)(SM_GET_PUBLIC_FIELD);
  register uintptr_t a0 asm ("a0") = (uint64_t)(PUBLIC_FIELD_PK_M);
  register uintptr_t a1 asm ("a1") = (uint64_t)(&pk_buffer);
  register uintptr_t a2 asm ("a2") = (uint64_t)(0x8000e000); // &enclave_handler_start
  asm volatile ( "jr a2; \n" \
        : "+r" (a0) \
        : "r" (a1), "r" (a2), "r" (a7) \
        : "memory"); \
  uint64_t bla = a0;
  */

  test_success();
}
