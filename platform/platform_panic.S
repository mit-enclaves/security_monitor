.option norvc
section .text

.globl platform_panic

platform_panic:
  # TODO cause all other cores to panic also
  j platform_panic
