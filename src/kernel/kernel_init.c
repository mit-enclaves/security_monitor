#include "kernel.h"

long disabled_hart_mask;

void kernel_init() {
  hls_init(0);
  query_clint(FDT_ADDR);
  query_harts(FDT_ADDR);

  // Initialize the device tree
  filter_and_copy_device_tree();
}
