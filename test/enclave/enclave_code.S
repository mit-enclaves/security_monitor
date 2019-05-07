.section ".enclave.text"
.enclave_entry:
   li a0, 5
   call dram_region_check_ownership
   call exit_enclave
