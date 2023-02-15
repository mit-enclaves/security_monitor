#include <sm.h>

void platform_interrupts_enter_enclave(thread_metadata_t *thread_metadata) {
  uint64_t mie_tmp = MIP_MTIP;
  thread_metadata->platform_csr.mie = swap_csr(mie, mie_tmp);
  thread_metadata->platform_csr.mideleg = swap_csr(mideleg, 0x0);
  thread_metadata->platform_csr.medeleg = swap_csr(medeleg, 0x0);
}

void platform_interrupts_exit_enclave(thread_metadata_t *thread_metadata) {
  write_csr(mie, thread_metadata->platform_csr.mie);
  write_csr(mideleg, thread_metadata->platform_csr.mideleg);
  write_csr(medeleg, thread_metadata->platform_csr.medeleg);
}
