#include <sm.h>

void platform_set_enclave_page_table(enclave_metadata_t *enclave_metadata) {
  // CSR MEATP
  write_csr(0x7c2, enclave_metadata->eptbr);
  swap_csr(satp, enclave_metadata->eptbr); // TODO hack
  
  return;
}
