.option norvc
.section .trap

.handle_mipi:
  LOAD t0, MENTRY_IPI_OFFSET(sp)
  sw x0, (t0)
  fence
# Now, decode the cause(s).
  STORE t1, 6*REGBYTES(sp)
  addi t0, sp, MENTRY_IPI_PENDING_OFFSET
  amoswap.w t0, x0, (t0)
  and t1, t0, IPI_SOFT
  beqz t1, 1f
  csrs mip, MIP_SSIP
1:
  andi t1, t0, IPI_FENCE_I
  beqz t1, 1f
  fence.i
1:
  andi t1, t0, IPI_SFENCE_VMA
  beqz t1, 1f
  sfence.vma
1:
  LOAD t1, 6*REGBYTES(sp)
  j .perform_mret
