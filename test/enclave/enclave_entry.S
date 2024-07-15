.section ".text.entry"

la sp, stack_top

la t0, _bss_start
la t1, _bss_end
beq t0, t1, 2f
1:sd zero, 0(t0)
addi t0, t0, 8
bne t0, t1, 1b
2:
mv t0, zero
mv t1, zero

la ra, enclave_entry
jr ra

# enclave_entry should not return!
.hang:
  j .hang
