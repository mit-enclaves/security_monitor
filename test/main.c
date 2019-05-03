#include "htif/htif.h"
#include <api.h>

__attribute__((section(".os.text")))

int main(void) {
  print_str("Hello World!\n");
  
  uint64_t ret = metadata_region_pages();
  print_int(ret);
  print_str(" is the number of metadata pages in a metadata region\n");
  
  print_str("flush_cached_dram_regions : ");
  api_result_t res = flush_cached_dram_regions();
  print_api_r(res);
  print_str("\n");

  print_str("block_dram_regions(3) : ");
  res = block_dram_region(3);
  print_api_r(res);
  print_str("\n");

  tohost = TOHOST_CMD(0, 0, 0b01);
}
