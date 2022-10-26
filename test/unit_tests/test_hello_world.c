#include <test.h>

void test_entry(int core_id, uintptr_t fdt_addr) {
  print_str("Hello World!\n");
  test_completed();
}
