.option norvc
.section .sm.text.untrusted_trap

.globl .illegal_insn_trap
.illegal_insn_trap:
  call .save_regs_and_set_args
  call illegal_insn_trap
  call .restore_regs
  j .perform_mret
