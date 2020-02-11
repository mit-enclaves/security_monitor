#include "kernel/mcall.h"
#include "kernel/kernel_api.h"
#include "htif/htif.h"
#include <errno.h>

// Code ispired by riscv-pk

static uintptr_t mcall_console_putchar(uint8_t ch);
static uintptr_t mcall_console_getchar();

void delegate_ecall_to_kernel(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
  uintptr_t code = regs[17];
  uint64_t arg0 = regs[10];

  uint64_t retval;

  switch (code)
  {
    case SBI_CONSOLE_PUTCHAR:
      retval = mcall_console_putchar(arg0);
      break;
    case SBI_CONSOLE_GETCHAR:
      retval = mcall_console_getchar();
      break;
    //case SBI_SEND_IPI:
    //case SBI_REMOTE_SFENCE_VMA:
    //case SBI_REMOTE_SFENCE_VMA_ASID:
    //case SBI_REMOTE_FENCE_I:
    //case SBI_CLEAR_IPI:
    /*
    case SBI_SHUTDOWN:
      retval = mcall_shutdown();
      break;
    case SBI_SET_TIMER:
      retval = mcall_set_timer(arg0);
      break;
    S*/
    default:
      retval = -ENOSYS;
      break;
  }

  regs[10] = retval;
}

static uintptr_t mcall_console_putchar(uint8_t ch)
{
  htif_putchar(ch);
  return 0;
}

static uintptr_t mcall_console_getchar()
{
  return htif_getchar();
}
