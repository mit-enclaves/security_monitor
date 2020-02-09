.option norvc
.section .text.clean_core

# void platform_clean_core(void); // arch state only
.globl platform_clean_core
platform_clean_core:
  # li x0, 0 # x0 is a zero constant, do not clean
  # li x1, 0 # x1 is $ra, do not clean
  # li x2, 0 # x2 is $sp, do not clean
  li x3, 0
  li x4, 0
  li x5, 0
  li x6, 0
  li x7, 0
  li x8, 0
  li x9, 0
  li x10, 0
  li x11, 0
  li x12, 0
  li x13, 0
  li x14, 0
  li x15, 0
  li x16, 0
  li x17, 0
  li x18, 0
  li x19, 0
  li x20, 0
  li x21, 0
  li x22, 0
  li x23, 0
  li x24, 0
  li x25, 0
  li x26, 0
  li x27, 0
  li x28, 0
  li x29, 0
  li x30, 0
  li x31, 0
  ret

  .globl platform_clean_core_leave_args
  platform_clean_core_leave_args:
    # li x0, 0 # x0 is a zero constant, do not clean
    # li x1, 0 # x1 is $ra, do not clean
    # li x2, 0 # x2 is $sp, do not clean
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    # li x10, 0 # x10 is $a0, do not clean
    # li x11, 0 # x11 is $a1, do not clean
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0
    ret
