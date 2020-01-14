.option norvc
.section .text

# void platform_jump_to_untrusted ( region_map_t * region_map, uint64_t virtual_pc, uint64_t virtual_sp ) __attribute__((noreturn));
.globl platform_jump_to_untrusted
platform_jump_to_untrusted:
  # $a0 contains the boot address
  mv ra, a0

  # Install page identity page tables
  la t0, platform_idpt
  ld t0, 0(t0)
  csrw satp, t0

  # Clean the cores
  la sp, 0
  call platform_clean_core
  call platform_purge_core
  ret
