#include <sm.h>

long disabled_hart_mask;

void platform_filter_and_copy_device_tree(void) {
  uintptr_t src_fdt_addr = FDT_ADDR;
  uintptr_t out_fdt_addr = platform_get_device_tree_addr();
  uint32_t size = fdt_size(src_fdt_addr);

  memcpy((void*)out_fdt_addr, (void*)src_fdt_addr, size);

  // TODO : here filter the device tree
  filter_harts(out_fdt_addr, &disabled_hart_mask);
  filter_plic(out_fdt_addr);
  //filter_compat(out_fdt_addr, "riscv,clint0");
  //filter_compat(out_fdt_addr, "riscv,debug-013");
}
