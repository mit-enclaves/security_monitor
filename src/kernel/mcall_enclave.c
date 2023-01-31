#include "kernel.h"

#include <errno.h>
#include <stdio.h>

// Code ispired by riscv-pk

static uintptr_t mcall_console_putchar(uint8_t ch);
static uintptr_t mcall_console_getchar();
static uintptr_t mcall_clear_ipi();

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
    case SBI_CLEAR_IPI:
      retval = mcall_clear_ipi();
      break;

    default:
      retval = -ENOSYS;
      printm("Wrong SBI system call code %d\n", code);
      uintptr_t mcause = read_csr(mcause);
      uintptr_t mepc = read_csr(mepc);
      uintptr_t mtval = read_csr(mtval);
      bad_trap(mcause, mepc, mtval);
      break;
  }

  regs[10] = retval;
}

static uintptr_t mcall_console_putchar(uint8_t ch)
{
  console_putchar(ch);
  return 0;
}

static uintptr_t mcall_console_getchar()
{
  return console_getchar();
}

static uintptr_t mcall_clear_ipi()
{
  return clear_csr(mip, MIP_SSIP) & MIP_SSIP;
}

void bad_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval) {
  console_init(); // HACK
  printm("Bad trap %ld at address %lx with mtval %lx\n", (int) mcause, mepc, mtval);
  platform_panic();
}
