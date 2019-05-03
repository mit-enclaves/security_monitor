#include "htif.h"

volatile uint64_t tohost    __attribute__((section(".htif.tohost")));
volatile uint64_t fromhost  __attribute__((section(".htif.fromhost")));

__attribute__((section(".os.text")))

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

void print_int(uint64_t n) {
   uint64_t ru = 1;
   for(uint64_t m = n; m > 1; m /= 10) {
      ru *= 10;
   }
   for(uint64_t i = ru; i >= 1; i /= 10) {
      char c = '0' + ((n / i) % 10);
      print_char(c);
   }
   return;
}

void print_api_r(api_result_t res) {
   switch(res) {
      case monitor_ok:
         print_str("monitor_ok");
         break;
      case monitor_invalid_value:
         print_str("monitor_invalid_value");
         break;
      case monitor_invalid_state:
         print_str("monitor_invalid_state");
         break;
      case monitor_concurrent_call:
         print_str("monitor_concurrent_call");
         break;
      case monitor_async_exit:
         print_str("monitor_async_exit");
         break;
      case monitor_access_denied:
         print_str("monitor_access_denied");
         break;
      case monitor_unsupported:
         print_str("monitor_unsupported");
         break;
      default:
         print_str("Unknown api_result code");
   }
   return;
}
