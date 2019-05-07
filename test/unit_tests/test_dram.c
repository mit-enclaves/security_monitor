#include "../htif/htif.h"
#include <api.h>

__attribute__((section(".illegal.data"))) int integer = 2019;

__attribute__((section(".os.text"))) int main(void) {
  print_str("Read integer stored in the 3rd DRAM region\n");
  print_int(integer);
  print_str("\n");

  print_str("assign_dram_region(3) : ");
  api_result_t res = assign_dram_region(3, 0);;
  print_api_r(res);
  print_str("\n");

  print_str("block_dram_region(3) : ");
  res = block_dram_region(3);
  print_api_r(res);
  print_str("\n");

  print_str("free_dram_region(3) : ");
  res = free_dram_region(3);;
  print_api_r(res);
  print_str("\n");

  print_str("Read integer stored in the 3rd DRAM region\n");
  print_int(integer);
  print_str("\n");
  
  tohost = TOHOST_CMD(0, 0, 0b01);
}



