.option norvc
.section .text.untrusted_trap

.globl .pmp_trap
.pmp_trap:
  call .save_regs_and_set_args
  call handle_trap_pmp
  call .restore_regs
  j .perform_mret
