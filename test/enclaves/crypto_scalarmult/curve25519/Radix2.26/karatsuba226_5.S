.text

.globl karatsuba226_255_5asm
.type karatsuba226_255_5asm, @function
karatsuba226_255_5asm:
    addi  sp, sp, -80   #
    sw    s0, 76(sp)    #
    sw    s1, 72(sp)    #
    sw    s2, 68(sp)    #
    sw    s3, 64(sp)    #
    sw    s4, 60(sp)    #
    sw    s5, 56(sp)    #
    sw    s6, 52(sp)    #
    sw    s7, 48(sp)    #
    sw    s8, 44(sp)    #
    sw    s9, 40(sp)    #
    sw    s10, 36(sp)   #
    sw    s11, 32(sp)   #
    sw    ra, 28(sp)    #

    mv    s9,  a2       # B pointer
    mv    s10, a1       # A pointer
    mv    s11, a0       # result pointer

    li    t6, 0x3ffffff # 2^26-1

    lw    a0, 0(s10)    # load A
    lw    a5, 0(s9)     # load B
    lw    a1, 4(s10)    #
    lw    a6, 4(s9)     #

    mul   t0, a0, a5    # A0*B0
    mulh  t1, a0, a5    #

    lw    a2, 8(s10)    #
    lw    a3, 12(s10)   #
    lw    a4, 16(s10)   #

    lw    a7, 8(s9)     #
    lw    s0, 12(s9)    #
    lw    s1, 16(s9)    #
    
    mul   t2, a0, a6    # A0*B1
    mulh  t3, a0, a6    #

    srli  t4, t0, 26    # 
    slli  t1, t1, 6     #
    and   s2, t0, t6    # r[0]
    or    s3, t1, t4    # r[1]

    mul   t0, a0, a7    # A0*B2
    mulh  t1, a0, a7    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    add   s3, s3, t2    # r[1] += A0*B1
    or    s4, t3, t4    # r[2]

    mul   t2, a0, s0    # A0*B3
    mulh  t3, a0, s0    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    add   s4, s4, t0    # r[2] += A0*B2
    or    s5, t1, t4    # r[3]

    mul   t0, a0, s1    # A0*B4
    mulh  t1, a0, s1    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    s6, t3, t4    # r[4]
    add   s5, s5, t2    # r[3] += A0*B3

    mul   t2, a1, a5    # A1*B0
    mulh  t3, a1, a5    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    s7, t1, t4    # r[5]
    add   s6, s6, t0    # r[4] += A0*B4

    mul   t0, a1, a6    # A1*B1
    mulh  t1, a1, a6    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    # 
    add   s3, s3, t2    # r[1] += A1*B0
    add   s4, s4, t3    # r[2] += A1*B0

    mul   t2, a1, a7    # A1*B2
    mulh  t3, a1, a7    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    # 
    add   s4, s4, t0    # r[2] += A1*B1
    add   s5, s5, t1    # r[3] += A1*B1

    mul   t0, a1, s0    # A1*B3
    mulh  t1, a1, s0    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    # 
    add   s5, s5, t2    # r[3] += A1*B2
    add   s6, s6, t3    # r[4] += A1*B2

    mul   t2, a1, s1    # A1*B4
    mulh  t3, a1, s1    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    # 
    add   s6, s6, t0    # r[4] += A1*B3
    add   s7, s7, t1    # r[5] += A1*B3

    mul   t0, a2, a5    # A2*B0
    mulh  t1, a2, a5    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    s8, t3, t4    # r[6]
    add   s7, s7, t2    # r[5] += A1*B4

    mul   t2, a2, a6    # A2*B1
    mulh  t3, a2, a6    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    # 
    add   s4, s4, t0    # r[2] += A2*B0
    add   s5, s5, t1    # r[3] += A2*B0

    mul   t0, a2, a7    # A2*B2
    mulh  t1, a2, a7    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    #
    add   s5, s5, t2    # r[3] += A2*B1
    add   s6, s6, t3    # r[4] += A2*B1

    mul   t2, a2, s0    # A2*B3
    mulh  t3, a2, s0    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    # 
    add   s6, s6, t0    # r[4] += A2*B2
    add   s7, s7, t1    # r[5] += A2*B2

    mul   t0, a2, s1    # A2*B4
    mulh  t1, a2, s1    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    #
    add   s7, s7, t2    # r[5] += A2*B3
    add   s8, s8, t3    # r[6] += A2*B3

    mul   t2, a3, a5    # A3*B0
    mulh  t3, a3, a5    # 

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    s9, t1, t4    # r[7]
    add   s8, s8, t0    # r[6] += A2*B4

    mul   t0, a3, a6    # A3*B1
    mulh  t1, a3, a6    # A3*B1

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    #
    add   s5, s5, t2    # r[3] += A3*B0
    add   s6, s6, t3    # r[4] += A3*B0

    mul   t2, a3, a7    # A3*B2
    mulh  t3, a3, a7    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    #
    add   s6, s6, t0    # r[4] += A3*B1
    add   s7, s7, t1    # r[5] += A3*B1

    mul   t0, a3, s0    # A3*B3
    mulh  t1, a3, s0    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    #
    add   s7, s7, t2    # r[5] += A3*B2
    add   s8, s8, t3    # r[6] += A3*B2

    mul   t2, a3, s1    # A3*B4
    mulh  t3, a3, s1    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    #
    add   s8, s8, t0    # r[6] += A3*B3
    add   s9, s9, t1    # r[7] += A3*B3

    mul   t0, a4, a5    # A4*B0
    mulh  t1, a4, a5    #

    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    s10, t3, t4   # r[8]
    add   s9, s9, t2    # r[7] += A3*B4

    mul   t2, a4, a6    # A4*B1
    mulh  t3, a4, a6    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    #
    add   s6, s6, t0    # r[4] += A4*B0
    add   s7, s7, t1    # r[5] += A4*B0

    mul   t0, a4, a7    # A4*B2
    mulh  t1, a4, a7    #
   
    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    # 
    add   s7, s7, t2    # r[5] += A4*B1
    add   s8, s8, t3    # r[6] += A4*B1

    mul   t2, a4, s0    # A4*B3
    mulh  t3, a4, s0    #

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t1, t1, t4    #
    add   s8, s8, t0    # r[6] += A4*B2
    add   s9, s9, t1    # r[7] += A4*B2

    mul   t0, a4, s1    # A4*B4
    mulh  t1, a4, s1    #
 
    srli  t4, t2, 26    #
    slli  t3, t3, 6     #
    and   t2, t2, t6    #
    or    t3, t3, t4    # 
    add   s9, s9, t2    # r[7] += A4*B3
    add   s10, s10, t3  # r[8] += A4*B3

    srli  t4, t0, 26    #
    slli  t1, t1, 6     #
    and   t0, t0, t6    #
    or    t5, t1, t4    # r[9]
    add   s10, s10, t0  # r[8] += A4*B4

    srli t0, s2, 26
    and  s2, s2, t6 
    add  s3, s3, t0

    srli t0, s3, 26
    and  s3, s3, t6
    add  s4, s4, t0

    srli t0, s4, 26
    and  s4, s4, t6
    add  s5, s5, t0

    srli t0, s5, 26
    and s5, s5, t6
    add s6, s6, t0

    srli t0, s6, 26
    and s6, s6, t6
    add s7, s7, t0

    srli t0, s7, 26
    and s7, s7, t6
    add s8, s8, t0

    srli t0, s8, 26
    and s8, s8, t6
    add s9, s9, t0

    srli t0, s9, 26
    and s9, s9, t6
    add s10, s10, t0

    srli t0, s10, 26
    and s10, s10, t6
    add t5, t5, t0

    srli t0, t5, 26
    and t5, t5, t6
    slli t1, t0, 1
    slli t2, t0, 4
    add t1, t1, t2
    add t0, t0, t2
    add s2, s2, t0

    sw    s2, 0(s11)    # store result
    sw    s3, 4(s11)    # 
    sw    s4, 8(s11)    #
    sw    s5, 12(s11)   #
    sw    s6, 16(s11)   #
    sw    s7, 20(s11)   #
    sw    s8, 24(s11)   #
    sw    s9, 28(s11)   #
    sw    s10,32(s11)   #
    sw    t5, 36(s11)   #

    lw    s0, 76(sp)    # restore variables
    lw    s1, 72(sp)    #
    lw    s11, 32(sp)   #
    lw    ra, 28(sp)    #
    lw    s2, 68(sp)    #
    lw    s3, 64(sp)    #
    lw    s4, 60(sp)    #
    lw    s5, 56(sp)    #
    lw    s6, 52(sp)    #
    lw    s7, 48(sp)    #
    lw    s8, 44(sp)    #
    lw    s9, 40(sp)    #
    lw    s10, 36(sp)   #
    addi  sp, sp, 80    #
    ret


