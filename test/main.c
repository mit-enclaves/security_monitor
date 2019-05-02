#include "htif/htif.h"
#include <api.h>

__attribute__((section(".os.text")))

int main(void) {
  print_str("Hello World!\n");
  uint64_t ret = thread_metadata_pages(); 
  print_str(ret);
  tohost = TOHOST_CMD(0, 0, 0b01);
}
