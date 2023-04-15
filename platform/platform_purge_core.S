#include <csr/csr.h>

.option norvc
.section .text.platform_purge_core

# void platform_purge_core (void); // uarch state only
.globl platform_purge_core
platform_purge_core:
  csrw CSR_MFLUSH, zero
  ret
