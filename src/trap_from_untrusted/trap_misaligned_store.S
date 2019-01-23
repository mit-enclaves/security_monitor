.option norvc
.section .trap

.misaligned_store_trap:
  call .save_regs_and_set_args
  call misaligned_store_trap
  call .restore_regs
  j .perform_mret
