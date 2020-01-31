#include <sm.h>

// TODO : Implement...

void platform_initialize_memory_protection(sm_state_t *sm) {
  write_csr(0x7c0, 0);                  //CSR_MEVBASE
  write_csr(0x7c1, 0xFFFFFFFFFFFFFFFF); //CSR_MEVMASK

  // (Untrusted has access to all the regions exept the first one, owned by the SM)
  uint64_t mmrbm = regions_to_bitmap(&(sm->untrusted_regions));
  write_csr(0x7c3, mmrbm); //CSR_MMRBM

  uint64_t mparbase = SM_STATE_ADDR;
  uint64_t mparmask = REGION_MASK;
  write_csr(0x7c5, mparbase); //CSR_MPARBASE
  write_csr(0x7c6, mparmask); //CSR_MPARMASK
}

void platform_protect_enclave_sm_handler(enclave_metadata_t *enclave_metadata, uintptr_t phys_addr) {
  enclave_metadata->platform_csr.meparbase = phys_addr;
  enclave_metadata->platform_csr.meparmask = 0x3FFF; // ~((1ul << (intlog2(size_handler) + 1)) - 1); // TODO how to not hardcode this?
  return;
}

void platform_memory_protection_enter_enclave(enclave_metadata_t *enclave_metadata) {

  swap_csr(0x7c0, enclave_metadata->platform_csr.ev_base); //CSR_MEVBASE
  swap_csr(0x7c1, enclave_metadata->platform_csr.ev_mask); //CSR_MEVMASK

  // TODO fix this
  uint64_t memrbm = regions_to_bitmap(&(enclave_metadata->regions));
  swap_csr(0x7c4, memrbm); //CSR_MEMRBM

  swap_csr(0x7c7, enclave_metadata->platform_csr.meparbase); //CSR_MEPARBASE
  swap_csr(0x7c8, enclave_metadata->platform_csr.meparmask); //CSR_MEPARMASK

  return;
}

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

void platform_hack_enclave_memory_protection(void) {
  write_csr(0x7c0,   0);
  write_csr(0x7c1,   0);

  write_csr(0x7c4,   0xFFFFFFFFFFFFFFFF);

  write_csr(0x7c7, 0);
  write_csr(0x7c8, 0xFFFFFFFFFFFFFFFF);
}

void platform_hack_exit_enclave_memory_protection(void) {
  write_csr(0x7c0,   0);
  write_csr(0x7c1,   0xFFFFFFFFFFFFFFFF);

  write_csr(0x7c4,   0);

  write_csr(0x7c7, 0);
  write_csr(0x7c8, 0);
}
