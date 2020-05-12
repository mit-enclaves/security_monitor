#include <sm.h>

void platform_set_enclave_page_table(enclave_metadata_t *enclave_metadata, thread_metadata_t *thread_metadata) {
  thread_metadata->platform_csr.eptbr = swap_csr(CSR_MEATP, enclave_metadata->platform_csr.eptbr);
  return;
}

void platform_restore_untrusted_page_table(thread_metadata_t *thread_metadata) {
  write_csr(CSR_MEATP, thread_metadata->platform_csr.eptbr);
  return;
}
