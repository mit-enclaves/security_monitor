#include <sm.h>

void platform_update_memory_protection(sm_state_t * sm, int core_id) {
  // Assumes that untrusted_state_lock and core lock are held

  uint64_t mmrbm = regions_to_bitmap(&(sm->untrusted_regions));
  uint64_t memrbm = 0UL;

  // If an enclave is running on the core, update memrbm.
  if(sm->cores[core_id].owner != OWNER_UNTRUSTED) {
    enclave_metadata_t *enclave = (enclave_metadata_t *) sm->cores[core_id].owner;
    memrbm = regions_to_bitmap(&(enclave->regions));
  }

  sm->cores[core_id].memrbm = memrbm;
  sm->cores[core_id].mmrbm = mmrbm;
  write_csr(CSR_MMRBM, mmrbm);
  write_csr(CSR_MEMRBM, memrbm);
}

void platform_update_untrusted_regions(sm_state_t* sm, int core_id, uint64_t index_id, bool flag) {
  // Assumes that untrusted_state_lock and core lock are held
  sm->untrusted_regions.flags[index_id] = flag;
  platform_update_memory_protection(sm, core_id);
}

void platform_update_enclave_regions(sm_state_t* sm, int core_id, enclave_metadata_t *enclave_metadata, uint64_t index_id, bool flag) {
  enclave_metadata->regions.flags[index_id] = flag;
  platform_update_memory_protection(sm, core_id);
}