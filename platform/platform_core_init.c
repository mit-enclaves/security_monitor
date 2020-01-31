#include <sm.h>

__attribute__((section(".text.platform_core_init")))

extern uint8_t platform_idpt;

void platform_core_init (void) {
  // Install identity page page tables with sv39 translation
  uint64_t satp_csr = (uint64_t)(&platform_idpt);
  satp_csr = satp_csr >> PAGE_SHIFT;
  satp_csr |= (SATP_MODE_SV39<<SATP_MODE); // sv39 translation
  write_csr(satp, satp_csr); // TODO: do not install IDPT for now
}
