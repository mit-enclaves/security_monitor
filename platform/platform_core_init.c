#include <sm.h>

__attribute__((section(".text.platform_core_init")))

extern uint8_t trap_vector_from_untrusted;

void platform_core_init (void) {
  // Set the interrupt handler address
  write_csr(mtvec, ( ((uint64_t)(&trap_vector_from_untrusted))&(~0x3L) ));
  
  // Initialize MSTATUS
  write_csr(mstatus, 0);

  // Enable user/supervisor use of perf counters
  write_csr(scounteren, -1);
  write_csr(mcounteren, -1);

  // Disable virtual memiry for now
  write_csr(satp, 0);

  // Enable Software Interrupts
  write_csr(mie, MIP_MSIP);
}
