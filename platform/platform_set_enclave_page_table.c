#include <sm.h>

void platform_set_enclave_page_table(enclave_metadata_t *enclave_metadata) {
  swap_csr(0x7c2, enclave_metadata->platform_csr.eptbr); //CSR_MEATP
  return;
}

void platfrom_restore_untrusted_page_table(enclave_metadata_t *enclave_metadata) {
  swap_csr(0x7c2, enclave_metadata->platform_csr.eptbr); //CSR_MEATP
  return;
}
