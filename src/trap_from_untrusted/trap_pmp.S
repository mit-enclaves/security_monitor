.option norvc
.section .trap

.handle_trap_pmp:
  call .save_regs_and_set_args
  call handle_trap_pmp
  call .restore_regs
  j .perform_mret
