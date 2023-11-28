#include <test.h>
#include <api_untrusted.h>

void test_entry(int core_id, uintptr_t fdt_addr) {
  console_init();

  // Set up an untrusted API call
  uint8_t pk_buffer[32];

  api_result_t result = SM_API_CALL(SM_GET_PUBLIC_FIELD, PUBLIC_FIELD_PK_M, &pk_buffer, 0, 0, 0, 0, 0);
  if(MONITOR_OK == result) {
    print_str("Test SUCCESS\n");
  }
  else {
    print_str("Test FAIL\n");
  }

  test_completed();
}
