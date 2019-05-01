#include <htif.h>
#include <api.h>

int main(void) {
  print_str("Hello World!\n");
  tohost = TOHOST_CMD(0, 0, 0b01); // report test done; 0 exit code
}
