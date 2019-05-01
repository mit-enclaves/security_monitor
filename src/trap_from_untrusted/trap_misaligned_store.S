.option norvc
.section .text.untrusted_trap 

.globl .misaligned_store_trap
.misaligned_store_trap:
  call .save_regs_and_set_args
  call misaligned_store_trap
  call .restore_regs
  j .perform_mret
