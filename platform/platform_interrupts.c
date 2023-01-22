#include <sm.h>

void platform_interrupts_enter_enclave(thread_metadata_t *thread_metadata) {
  uint64_t mie_tmp = MIP_MTIP;
  thread_metadata->platform_csr.mie = swap_csr(mie, mie_tmp);
}

void platform_interrupts_exit_enclave(thread_metadata_t *thread_metadata) {
  write_csr(mie, thread_metadata->platform_csr.mie);
}
