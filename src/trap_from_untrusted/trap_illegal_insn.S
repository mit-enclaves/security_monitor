.option norvc
.section .trap

.illegal_insn_trap:
  call .save_regs_and_set_args
  call illegal_insn_trap
  call .restore_regs
  j .perform_mret
