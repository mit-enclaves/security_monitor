#include "kernel.h"

#include <errno.h>
#include <stdio.h>

// Code ispired by riscv-pk

static uintptr_t mcall_console_putchar(uint8_t ch);
static uintptr_t mcall_console_getchar();
static uintptr_t mcall_set_timer(uint64_t when);
static uintptr_t mcall_clear_ipi();

void delegate_ecall_to_kernel(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc) {
  uintptr_t code = regs[17];
  uint64_t arg0 = regs[10];

  uint64_t retval;
  uintptr_t ipi_type;

  switch (code)
  {
    case SBI_CONSOLE_PUTCHAR:
      retval = mcall_console_putchar(arg0);
      break;
    case SBI_CONSOLE_GETCHAR:
      retval = mcall_console_getchar();
      break;
    case SBI_SEND_IPI:
      ipi_type = IPI_SOFT;
      goto send_ipi;
    case SBI_REMOTE_SFENCE_VMA:
    case SBI_REMOTE_SFENCE_VMA_ASID:
      ipi_type = IPI_SFENCE_VMA;
      goto send_ipi;
    case SBI_REMOTE_FENCE_I:
      ipi_type = IPI_FENCE_I;
send_ipi:
      send_ipi_many((uintptr_t*)arg0, ipi_type);
      retval = 0;
      break;
    case SBI_CLEAR_IPI:
      retval = mcall_clear_ipi();
      break;
/*
    case SBI_SHUTDOWN:
      retval = mcall_shutdown();
      break; */
    case SBI_EXIT:
      send_exit_cmd(arg0);
      retval = 0; 
      break;
    case SBI_SET_TIMER:
      retval = mcall_set_timer(arg0);
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

static uintptr_t mcall_set_timer(uint64_t when)
{
  *HLS()->timecmp = when;
  clear_csr(mip, MIP_STIP);
  set_csr(mie, MIP_MTIP);
  return 0;
}

void send_ipi(uintptr_t recipient, int event)
{
  //if (((disabled_hart_mask >> recipient) & 1)) return;
  atomic_or(&OTHER_HLS(recipient)->mipi_pending, event);
  mb();
  *OTHER_HLS(recipient)->ipi = 1;
}

void send_ipi_many(uintptr_t* pmask, int event)
{
  uintptr_t mask = hart_mask;
  if (pmask)
    mask &= load_uintptr_t(pmask, read_csr(mepc));

  // send IPIs to everyone
  for (uintptr_t i = 0, m = mask; m; i++, m >>= 1)
    if (m & 1)
      send_ipi(i, event);

  if (event == IPI_SOFT)
    return;

  // wait until all events have been handled.
  // prevent deadlock by consuming incoming IPIs.
  uint32_t incoming_ipi = 0;
  for (uintptr_t i = 0, m = mask; m; i++, m >>= 1)
    if (m & 1)
      while (*OTHER_HLS(i)->ipi)
        incoming_ipi |= atomic_swap(HLS()->ipi, 0);

  // if we got an IPI, restore it; it will be taken after returning
  if (incoming_ipi) {
    *HLS()->ipi = incoming_ipi;
    mb();
  }
}

void bad_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval) {
  printm("Bad trap %ld at address %lx with mtval %lx\n", (int) mcause, mepc, mtval);
  platform_panic();
}
