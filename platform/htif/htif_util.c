#include "htif.h"
#include <platform_assert.h>

// Code strongly inspired by riscv-pk github.com/riscv/riscv-pk/blob/master/machine/htif.c

volatile int htif_console_buf = 0; // TODO WARNING this will end up in bss

#define TOHOST(base_int)	(uint64_t *)(base_int + TOHOST_OFFSET)
#define FROMHOST(base_int)	(uint64_t *)(base_int + FROMHOST_OFFSET)

#define TOHOST_OFFSET		((uintptr_t)*tohost - (uintptr_t)__htif_base)
#define FROMHOST_OFFSET		((uintptr_t)*fromhost - (uintptr_t)__htif_base)

static void __check_fromhost() { // Code taken from riscv-pk
  uint64_t fh = *fromhost;
  if (!fh)
    return;
  *fromhost = 0;

  // this should be from the console
  assert(FROMHOST_DEV(fh) == 1);
  switch (FROMHOST_CMD(fh)) {
    case 0:
      htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
      break;
    case 1:
      break;
    default:
      break;
      assert(0);
  }
}

static void __set_tohost(uintptr_t dev, uintptr_t cmd, uintptr_t data)
{
  while (*tohost)
    __check_fromhost();
  *tohost = TOHOST_CMD(dev, cmd, data);
}


uint64_t htif_getchar() {
    __check_fromhost();
    int ch = htif_console_buf;
    if (ch >= 0) {
      htif_console_buf = -1;
      __set_tohost(1, 0, 0);
    }

  return ch - 1;
}

void htif_putchar(uint8_t c) {
  __set_tohost(1, 1, c);
}

/*
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
*/
