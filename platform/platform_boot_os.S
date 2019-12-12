.option norvc
section .text

.globl platform_boot_os

platform_boot_os:
  # $a0 contains the boot address
  mv ra, a0

  # Install page identity page tables
  la t0, platform_satp_idpt
  ld t0, 0(t0)
  csrw t0, satp

  # Clean the cores
  la sp, 0
  call platform_clean_core
  call platform_purge_core
  return
