#include <sm.h>

void platform_set_enclave_page_table(enclave_metadata_t *enclave_metadata) {
  swap_csr(CSR_MEATP, enclave_metadata->platform_csr.eptbr);
  return;
}

void platfrom_restore_untrusted_page_table(enclave_metadata_t *enclave_metadata) {
  swap_csr(CSR_MEATP, enclave_metadata->platform_csr.eptbr);
  return;
}
