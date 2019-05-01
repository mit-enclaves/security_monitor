#include "htif.h"

volatile uint64_t tohost    __attribute__((section(".htif.tohost")));
volatile uint64_t fromhost  __attribute__((section(".htif.fromhost")));

__attribute__((section(".text.os")))

void print_char(char c) {
  // No synchronization needed, as the bootloader runs solely on core 0

  while (tohost) {
    // spin
    fromhost = 0;
  }

  tohost = TOHOST_CMD(1, 1, c); // send char
}

void print_str(char* s) {
  while (*s != 0) {
    print_char(*s++);
  }
}
