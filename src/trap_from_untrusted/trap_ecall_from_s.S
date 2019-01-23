.option norvc
.section .trap

.ecall_from_s_trap:
  call .save_regs_and_set_args
  call ecall_from_s_trap
  call .restore_regs
  j .perform_mret
