#include <sm.h>

void platform_memory_protection_exit_enclave(sm_state_t *sm, int core_id, thread_metadata_t *thread_metadata) {
  // Assume the core lock is held
  write_csr(CSR_MEVBASE, thread_metadata->platform_csr.ev_base);
  write_csr(CSR_MEVMASK, thread_metadata->platform_csr.ev_mask);

  write_csr(CSR_MEMRBM, thread_metadata->platform_csr.memrbm);
  sm->cores[core_id].memrbm = thread_metadata->platform_csr.memrbm;

  write_csr(CSR_MEPARBASE, thread_metadata->platform_csr.meparbase);
  write_csr(CSR_MEPARMASK, thread_metadata->platform_csr.meparmask);

  return;
}
