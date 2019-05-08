.section ".text.enclave_code"
.enclave_entry:
   li a0, 5
.loop:
   add a0, a0, 1
   j .loop
