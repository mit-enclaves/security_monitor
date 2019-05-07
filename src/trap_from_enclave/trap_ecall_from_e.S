.option norvc
.section .sm.text.enclave_trap 

.globl .ecall_from_e_trap
.ecall_from_e_trap:
  call .enclave_save_regs_and_set_args
  call ecall_from_enclave_trap
  call .enclave_restore_regs
  j .enclave_perform_mret
