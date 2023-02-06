.section ".text.entry"

la sp, stack_top
la ra, enclave_entry
jr ra

# enclave_entry should not return!
.hang:
  j .hang
