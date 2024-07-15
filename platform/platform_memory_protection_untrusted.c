#include <sm.h>

void platform_initialize_memory_protection(sm_state_t *sm, int core_id) {
  
  // Install identity page page tables with sv39 translation
  uint64_t satp_csr = (uint64_t)(IDPT_BASE);
  satp_csr = satp_csr >> PAGE_SHIFT;
  satp_csr |= (SATP_MODE_SV39<<SATP_MODE); // sv39 translation
  write_csr(satp, satp_csr);

  write_csr(CSR_MEVBASE, MVBASE_DEFAULT);
  write_csr(CSR_MEVMASK, MVMASK_DEFAULT);

  platform_update_memory_protection(sm, core_id);

  uint64_t mparbase = SM_ADDR;
  uint64_t mparmask = REGION_MASK;
  write_csr(CSR_MPARBASE, mparbase);
  write_csr(CSR_MPARMASK, mparmask);
}

void platform_protect_enclave_sm_handler(enclave_metadata_t *enclave_metadata, uintptr_t phys_addr) {
  enclave_metadata->platform_csr.meparbase = phys_addr;
  enclave_metadata->platform_csr.meparmask = ~((~HANDLER_LEN) % HANDLER_LEN); 
  // ~((1ul << (intlog2(size_handler) + 1)) - 1);
  return;
}

void platform_memory_protection_enter_enclave(sm_core_t *core, enclave_metadata_t *enclave_metadata, thread_metadata_t *thread_metadata) {

  thread_metadata->platform_csr.ev_base =
    swap_csr(CSR_MEVBASE, enclave_metadata->platform_csr.ev_base);
  thread_metadata->platform_csr.ev_mask =
    swap_csr(CSR_MEVMASK, enclave_metadata->platform_csr.ev_mask);

  uint64_t memrbm = regions_to_bitmap(&(enclave_metadata->regions));
  
  while(platform_lock_acquire(&core->lock) != 0) {};
  core->memrbm = memrbm;
  thread_metadata->platform_csr.memrbm =
    swap_csr(CSR_MEMRBM, memrbm);
  platform_lock_release(&core->lock);

  thread_metadata->platform_csr.meparbase =
    swap_csr(CSR_MEPARBASE, enclave_metadata->platform_csr.meparbase);
  thread_metadata->platform_csr.meparmask =
    swap_csr(CSR_MEPARMASK, enclave_metadata->platform_csr.meparmask);

  return;
}