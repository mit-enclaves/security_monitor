#include <sm.h>

void platform_memory_protection_exit_enclave(enclave_metadata_t *enclave_metadata) {

  swap_csr(0x7c0, enclave_metadata->platform_csr.ev_base); //CSR_MEVBASE
  swap_csr(0x7c1, enclave_metadata->platform_csr.ev_mask); //CSR_MEVMASK

  // TODO fix this
  uint64_t memrbm = regions_to_bitmap(&(enclave_metadata->regions));
  swap_csr(0x7c4, memrbm); //CSR_MEMRBM

  swap_csr(0x7c7, enclave_metadata->platform_csr.meparbase); //CSR_MEPARBASE
  swap_csr(0x7c8, enclave_metadata->platform_csr.meparmask); //CSR_MEPARMASK

  return;
}
