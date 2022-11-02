.globl toradix28asm
.type toradix28asm, @function
toradix28asm:
    li   t4, 0xFF
    lw   t1, 32(a0)
    sw   t1, 64(a0)
    lw   t1, 28(a0)
    srli t2, t1, 8
    sw   t2, 60(a0)
    and  t3, t1, t4
    sw   t3, 56(a0)
   
    lw   t1, 24(a0)
    srli t2, t1, 8
    sw   t2, 52(a0)
    and  t3, t1, t4
    sw   t3, 48(a0)
   
    lw   t1, 20(a0)
    srli t2, t1, 8
    sw   t2, 44(a0)
    and  t3, t1, t4
    sw   t3, 40(a0)
   
    lw   t1, 16(a0)
    srli t2, t1, 8
    sw   t2, 36(a0)
    and  t3, t1, t4
    sw   t3, 32(a0)
   
    lw   t1, 12(a0)
    srli t2, t1, 8
    sw   t2, 28(a0)
    and  t3, t1, t4
    sw   t3, 24(a0)
   
    lw   t1, 8(a0)
    srli t2, t1, 8
    sw   t2, 20(a0)
    and  t3, t1, t4
    sw   t3, 16(a0)
   
    lw   t1, 4(a0)
    srli t2, t1, 8
    sw   t2, 12(a0)
    and  t3, t1, t4
    sw   t3, 8(a0)
          
    lw   t1, 0(a0)
    srli t2, t1, 8
    sw   t2, 4(a0)
    and  t3, t1, t4
    sw   t3, 0(a0)
    ret
