#include "htif/htif.h"
#include <api.h>

__attribute__((section(".text.os")))

int main(void) {
  print_str("Hello World!\n");
  tohost = TOHOST_CMD(0, 0, 0b01);
}
