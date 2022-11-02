.section ".text.entry"

la ra, enclave_entry
jr ra

# enclave_entry should not return!
.hang:
  j .hang
