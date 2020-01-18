.option norvc
.section .text.platform_core_init

# void platform_core_init ();
.globl platform_core_init
platform_core_init:
  # any platform-specific core initialization happens here

  # Install page identity page tables, with sv39 translation
  la t0, platform_idpt
  ld t0, 0(t0)
  li t1, (8<<60) # sv39
  or t1, t1, t0
  csrw satp, t1

  ret
