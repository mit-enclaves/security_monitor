.option norvc
.section .text.sm.untrusted_trap 

.globl .ecall_from_s_trap
.ecall_from_s_trap:
  call .save_regs_and_set_args
  call ecall_from_s_trap
  call .restore_regs
  j .perform_mret
