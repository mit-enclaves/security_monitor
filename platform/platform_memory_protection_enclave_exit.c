#include <sm.h>

void platform_memory_protection_exit_enclave(enclave_metadata_t *enclave_metadata) {

  swap_csr(CSR_MEVBASE, enclave_metadata->platform_csr.ev_base);
  swap_csr(CSR_MEVMASK, enclave_metadata->platform_csr.ev_mask);

  uint64_t memrbm = regions_to_bitmap(&(enclave_metadata->regions));
  swap_csr(CSR_MEMRBM, memrbm);

  swap_csr(CSR_MEPARBASE, enclave_metadata->platform_csr.meparbase);
  swap_csr(CSR_MEPARMASK, enclave_metadata->platform_csr.meparmask);

  return;
}
