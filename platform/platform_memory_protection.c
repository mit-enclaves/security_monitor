#include <sm.h>

// TODO : Implement...

void platform_protect_enclave_sm_handler(uintptr_t phys_addr, uint64_t size_handler) {
  //enclave->meparbase = phys_addr;
  //enclave->meparmask = ~((1ul << (intlog2(size_handler) + 1)) - 1);
  return;
}

void platform_protect_memory_enter_enclave(enclave_metadata_t *enclave_metadata) {
  /*
  swap_csr(CSR_MEVBASE, enclave_metadata->platform_csr->evbase);
  swap_csr(CSR_MEVMASK, enclave_metadata->platform_csr->evmask);

  swap_csr(CSR_MEMRBM, enclave_metadata->platform_csr->dram_bitmap);

  swap_csr(CSR_MEPARBASE, enclave_metadata->platform_csr->meparbase);
  swap_csr(CSR_MEPARMASK, enclave_metadata->platform_csr->meparmask);
  */
  return;
}
