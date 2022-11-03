.text

.globl mulmod226asm
.type mulmod226asm, @function
mulmod226asm:
    addi  sp, sp, -48  # space for hr array and pushes
    sw    ra, 40(sp)   #
    sw    s0, 36(sp)   #
    sw    s1, 32(sp)   #
    sw    s2, 28(sp)   #
    sw    s3, 24(sp)   #
    sw    s4, 20(sp)   #
    sw    s5, 16(sp)   #
    sw    s6, 12(sp)   #
    sw    s7, 8(sp)    #
    sw    s8, 4(sp)    #
    sw    s9, 0(sp)    #

    mv    s8, a0       # store A address
    lwu    a5, 0(a1)    # load B 
    lwu    a6, 4(a1)    #
    lwu    s0, 8(a1)    #
    lwu    s1, 12(a1)   #
    lwu    s2, 16(a1)   #
    
    lwu    a0, 0(s8)    # load A
    lwu    a1, 4(s8)    #
    lwu    a2, 8(s8)    #
    lwu    a3, 12(s8)   #
    lwu    a4, 16(s8)   #

    mul   t0, a0, a5   # A_0*B_0
    mulh  t1, a0, a5   #
    li    t6, 0x3ffffff # 2^26-1
    li    t5, 5        # constant c

    mul   t3, a0, a6   # A_0*B_1
    mulh  t4, a0, a6   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    s4, t1, t2   # r_1
    and   s3, t0, t6   # r_0
    
    mul   t0, a1, a5   # A_1*B_0
    mulh  t1, a1, a5   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    and   t3, t3, t6   #
    or    s5, t4, t2   # r_2
    add   s4, s4, t3   # r_1 += A_0*B_1

    mul   t3, a0, s0   # A_0*B_2
    mulh  t4, a0, s0   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s4, s4, t0   # r_1 += A_1*B_0
    add   s5, s5, t1   # r_2 += A_1*B_0

    mul   t0, a1, a6   # A_1*B_1
    mulh  t1, a1, a6   # 
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    and   t3, t3, t6   #
    or    s6, t4, t2   # r_3
    add   s5, s5, t3   # r_2 += A_0*B_2

    mul   t3, a2, a5   # A_2*B_0
    mulh  t4, a2, a5   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s6, s6, t1   # r_3 += A_1*B_1
    add   s5, s5, t0   # r_2 += A_1*B_1

    mul   t0, a0, s1   # A_0 * B_3
    mulh  t1, a0, s1   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   s6, s6, t4   # r_3 += A_2*B_0
    add   s5, s5, t3   # r_2 += A_2*B_0

    mul   t3, a1, s0   # A_1*B_2
    mulh  t4, a1, s0   #
    srli  t2, t0, 26   # 
    slli  t1, t1, 6    #
    and   t0, t0, t6   #
    or    s7, t1, t2   # r_4
    add   s6, s6, t0   # r_3 += A_0*B_3

    mul   t0, a2, a6   # A_2*B_1
    mulh  t1, a2, a6   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   s7, s7, t4   # r_4 += A_1*B_2
    add   s6, s6, t3   # r_3 += A_1*B_2

    mul   t3, a3, a5   # A_3*B_0
    mulh  t4, a3, a5   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s7, s7, t1   # r_4 += A_2*B_1
    add   s6, s6, t0   # r_3 += A_2*B_1

    mul   t0, a0, s2   # A_0*B_4
    mulh  t1, a0, s2   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   s7, s7, t4   # r_4 += A_3*B_0
    add   s6, s6, t3   # r_3 += A_3*B_0

    mul   t3, a1, s1   # A_1*B_3
    mulh  t4, a1, s1   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    and   t0, t0, t6   #
    or    s9, t1, t2   # overflow
    add   s7, s7, t0   # r_4 += A_0*B_4

    mul   t0, a2, s0   # A_2*B_2
    mulh  t1, a2, s0   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   s9, s9, t4   # overflow
    add   s7, s7, t3   # r_4 += A_1*B_3

    mul   t3, a3, a6   # A_3*B_1
    mulh  t4, a3, a6   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s9, s9, t1   # overflow
    add   s7, s7, t0   # r_4 += A_2*B_2

    mul   t0, a4, a5   # A_4*B_0
    mulh  t1, a4, a5   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   s9, s9, t4   # overflow
    add   s7, s7, t3   # r_4 += A_3*B_1

    mul   t3, a1, s2   # A_1*B_4
    mulh  t4, a1, s2   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s9, s9, t1   # overflow
    add   s7, s7, t0   # r_4 += A_4*B_0

    mul   t0, a2, s1   # A_2*B_3
    mulh  t1, a2, s1   #
    srli  t2, t3, 26   # 
    slli  t4, t4, 6    #
    and   t3, t3, t6   #
    or    a7, t4, t2   # r_1*c(variable name)
    add   s9, s9, t3   # overflow

    mul   t3, a3, s0   # A_3*B_2
    mulh  t4, a3, s0   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   a7, a7, t1   # r_1*c += A_2*B_3
    add   s9, s9, t0   # overflow

    mul   t0, a4, a6   # A_4*B_1
    mulh  t1, a4, a6   # 
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    and   t3, t3, t6   #
    add   a7, a7, t4   # r_1*c += A_3*B_2
    add   s9, s9, t3   # overflow
    and   t3, t0, t6   #
    add   s9, s9, t3   # overflow

    mul   t4, s9, t5   # 5*overflow
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    mul   a5, a2, s2   # A_2*B_4, a5(B_0) is not needed anymore and used as general register
    mulh  a6, a2, s2   # a6(B_1) is not needed anymore and used as gerenal register

    add   a7, a7, t1   # r_1*c += A_4*B_1
    mul   t0, a3, s1   # A_3*B_3
    mulh  t1, a3, s1   #

    add   a0, s3, t4   # 
    srli  t2, a5, 26   #
    slli  a6, a6, 6    #
    and   a5, a5, t6   #

    or    s9, a6, t2   # r_2*c(variable name)
    add   a7, a7, a5   # r_1*c += A_2*B_4

    mul   t3, a4, s0   # A_4*B_2
    mulh  t4, a4, s0   #
    srli  t2, t0, 26   #
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    and   t0, t0, t6   #
    add   s9, s9, t1   # r_2*c += A_3*B_3
    add   a7, a7, t0   # r_1*c += A_3*B_3
    srli  t2, t3, 26   #
    and   t3, t3, t6   #
    add   a7, a7, t3   # r_1*c += A_4*B_2

    mul   a5, a7, t5   # r_1*c *= 5
    mul   t0, a3, s2   # A_3*B_4
    mulh  t1, a3, s2   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    add   s9, s9, t4   # r_2*c += A_4*B_2
    add   a1, s4, a5   # r_1 += r_1*c 

    mul   t3, a4, s1   # A_4*B_3
    mulh  t4, a4, s1   #
    srli  t2, t0, 26   # 
    slli  t1, t1, 6    #
    and   t0, t0, t6   #
    or    a7, t1, t2   # r_3*c(variable name)
    add   s9, s9, t0   # r_2*c += A_3*B_4
    and   t2, t3, t6   #
    add   s9, s9, t2   # r_2*c += A_4*B_3

    mul   a5, s9, t5   # r_2*c *= 5
    mul   t0, a4, s2   # A_4*B_4
    mulh  t1, a4, s2   #
    srli  t2, t3, 26   #
    slli  t4, t4, 6    #
    or    t4, t4, t2   #
    add   a2, s5, a5   # r_2 += r_2*c
    add   a7, a7, t4   # r_3*c += A_4*B_3
    srli  t2, t0, 26   #
    and   t0, t0, t6   #
    add   a7, a7, t0   # r_3*c += A_4*B_4 
    
    mul   a7, a7, t5   # r_3*c *= 5
    slli  t1, t1, 6    #
    or    t1, t1, t2   #
    mul   t1, t1, t5   # (A_4*B_4)_h *=5
    add   a3, s6, a7   # r_3 += r_3*c
    add   a4, s7, t1   # r_4 += (A_4*B_4)_h*5

    call  squeeze226param

    sw    a0, 0(s8)    # store result
    sw    a1, 4(s8)    #
    sw    a2, 8(s8)    #
    sw    a3, 12(s8)   #
    sw    a4, 16(s8)   #

    lwu    ra, 40(sp)   # restore variables
    lwu    s0, 36(sp)   #
    lwu    s1, 32(sp)   #
    lwu    s2, 28(sp)   #
    lwu    s3, 24(sp)   #
    lwu    s4, 20(sp)   #
    lwu    s5, 16(sp)   #
    lwu    s6, 12(sp)   #
    lwu    s7, 8(sp)    #
    lwu    s8, 4(sp)    #
    lwu    s9, 0(sp)    #
    addi  sp, sp, 48   #
    ret
