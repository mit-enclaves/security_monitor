#include <sm.h>

__attribute__((section(".text.platform_init")))

volatile uint64_t* mtime;

void platform_init() {
  query_clint(FDT_ADDR);
}
