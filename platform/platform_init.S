.option norvc
.section .text.platform_init

# IMPORTANT: this routine is invoked ONCE, by Core 0. All other cores are asleep.

# void platform_init ();
.globl platform_init
platform_init:
  # any platform-specific initialization happens here
  ret
