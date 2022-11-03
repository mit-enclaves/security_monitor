.text

#x_0..x_15 = t2..t6 a4..a7 s0..s6
.globl crypto_core_salsa208
.type crypto_core_salsa208, @function
crypto_core_salsa208:
    addi  sp, sp, -96   # store variables
    sw    s0, 28(sp)    #
    sw    s1, 24(sp)    #
    sw    s2, 20(sp)    #
    sw    s3, 16(sp)    #
    sw    s4, 12(sp)    #
    sw    s5, 8(sp)     #
    sw    s6, 4(sp)     #
    sw    s7, 0(sp)     #

    lbu   t0, 3(a3)     #
    lbu   t1, 2(a3)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 1(a3)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 0(a3)     #
    slli  t0, t0, 8     #
    or    t2, t0, t1    # x0

    lbu   t0, 3(a2)     #
    lbu   t1, 2(a2)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 1(a2)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 0(a2)     #
    slli  t0, t0, 8     #
    or    t3, t0, t1    # x1

    lbu   t0, 7(a2)     #
    lbu   t1, 6(a2)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 5(a2)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 4(a2)     #
    slli  t0, t0, 8     #
    or    t4, t0, t1    # x2

    lbu   t0, 11(a2)    #
    lbu   t1, 10(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 9(a2)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 8(a2)     #
    slli  t0, t0, 8     #
    or    t5, t0, t1    # x3

    lbu   t0, 15(a2)    #
    lbu   t1, 14(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 13(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 12(a2)    #
    slli  t0, t0, 8     #
    or    t6, t0, t1    # x4

    lbu   t0, 7(a3)     #
    lbu   t1, 6(a3)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 5(a3)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 4(a3)     #
    slli  t0, t0, 8     #
    or    a4, t0, t1    # x5

    lbu   t0, 3(a1)     #
    lbu   t1, 2(a1)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 1(a1)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 0(a1)     #
    slli  t0, t0, 8     #
    or    a5, t0, t1    # x6

    lbu   t0, 7(a1)     #
    lbu   t1, 6(a1)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 5(a1)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 4(a1)     #
    slli  t0, t0, 8     #
    or    a6, t0, t1    # x7

    lbu   t0, 11(a1)    #
    lbu   t1, 10(a1)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 9(a1)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 8(a1)     #
    slli  t0, t0, 8     #
    or    a7, t0, t1    # x8

    lbu   t0, 15(a1)    #
    lbu   t1, 14(a1)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 13(a1)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 12(a1)    #
    slli  t0, t0, 8     #
    or    s0, t0, t1    # x9

    lbu   t0, 11(a3)    #
    lbu   t1, 10(a3)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 9(a3)     #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 8(a3)     #
    slli  t0, t0, 8     #
    or    s1, t0, t1    # x10

    lbu   t0, 19(a2)    #
    lbu   t1, 18(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 17(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 16(a2)    #
    slli  t0, t0, 8     #
    or    s2, t0, t1    # x11

    lbu   t0, 23(a2)    #
    lbu   t1, 22(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 21(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 20(a2)    #
    slli  t0, t0, 8     #
    or    s3, t0, t1    # x12

    lbu   t0, 27(a2)    #
    lbu   t1, 26(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 25(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 24(a2)    #
    slli  t0, t0, 8     #
    or    s4, t0, t1    # x13

    lbu   t0, 31(a2)    #
    lbu   t1, 30(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 29(a2)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 28(a2)    #
    slli  t0, t0, 8     #
    or    s5, t0, t1    # x14

    lbu   t0, 15(a3)    #
    lbu   t1, 14(a3)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 13(a3)    #
    slli  t0, t0, 8     #
    or    t0, t0, t1    #
    lbu   t1, 12(a3)    #
    slli  t0, t0, 8     #
    or    s6, t0, t1    # x15

    sw    t2, 32(sp)    # store j values
    sw    t3, 36(sp)    #
    sw    t4, 40(sp)    #
    sw    t5, 44(sp)    #
    sw    t6, 48(sp)    #
    sw    a4, 52(sp)    #
    sw    a5, 56(sp)    #
    sw    a6, 60(sp)    #
    sw    a7, 64(sp)    #
    sw    s0, 68(sp)    #
    sw    s1, 72(sp)    #
    sw    s2, 76(sp)    #
    sw    s3, 80(sp)    #
    sw    s4, 84(sp)    #
    sw    s5, 88(sp)    #
    sw    s6, 92(sp)    #

    li    s7, 8
    
loop:
    add   t0, t2, s3    # x0+x12
    slli  t1, t0, 7     # x0+x12 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   t6, t6, t0    # x4 ^= rot7(x0+x12)

    add   t0, t6, t2    # x4+x0
    slli  t1, t0, 9     # x4+x0 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   a7, a7, t0    # x8 ^= rot9(x4+x0)

    add   t0, a7, t6    # x8 + x4
    slli  t1, t0, 13    # x8 + x4 rot13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   s3, s3, t0    # x12 ^= rot13(x8+x4)

    add   t0, s3, a7    # x12 + x8
    slli  t1, t0, 18    # x12 + x8 rot18
    srli  t0, t0, 14    # 
    or    t0, t0, t1    #
    xor   t2, t2, t0    # x0 ^= rot18(x12+x8)

    add   t0, a4, t3    # x5 + x1
    slli  t1, t0, 7     # x5 + x1 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   s0, s0, t0    # x9 ^= rot7(x5+x1)

    add   t0, s0, a4    # x9 + x5
    slli  t1, t0, 9     # x9 + x5 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   s4, s4, t0    # x13 ^= rot9(x9+x5)

    add   t0, s4, s0    # x13 + x9
    slli  t1, t0, 13    # x13 + x9 rot13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   t3, t3, t0    # x1 ^= rot13(x13+x9)

    add   t0, t3, s4    # x1 + x13
    slli  t1, t0, 18    # x1 + x13 rot18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   a4, a4, t0    # x5 ^= rot18(x1+x13)

    add   t0, s1, a5    # x10 + x6
    slli  t1, t0, 7     # x10 + x6 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   s5, s5, t0    # x14 ^= rot7(x10+x6)

    add   t0, s5, s1    # x14 + x10
    slli  t1, t0, 9     # x14 + x10 rot9
    srli  t0, t0, 23    # 
    or    t0, t0, t1    #
    xor   t4, t4, t0    # x2 ^= rot9(x14+x10)

    add   t0, t4, s5    # x2 + x14
    slli  t1, t0, 13    # x2 + x14 rot13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   a5, a5, t0    # x6 ^= rot13(x2+x14)

    add   t0, a5, t4    # x6 + x2
    slli  t1, t0, 18    # x6 + x2 rot18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   s1, s1, t0    # x10 ^= rot18(x6+x2)

    add   t0, s6, s2    # x15 + x11
    slli  t1, t0, 7     # x15 + x11 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   t5, t5, t0    # x3 ^= rot7(x15+x11)

    add   t0, t5, s6    # x3 + x15
    slli  t1, t0, 9     # x3 + x15 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   a6, a6, t0    # x7 ^= rot9(x3+x15)

    add   t0, a6, t5    # x7 + x3
    slli  t1, t0, 13    # x7 + x3 rot13
    srli  t0, t0, 19    # 
    or    t0, t0, t1    #
    xor   s2, s2, t0    # x11 ^= rot13(x7+x3)

    add   t0, s2, a6    # x11 + x7
    slli  t1, t0, 18    # x11 + x7 rot18
    srli  t0, t0, 14    # 
    or    t0, t0, t1    #
    xor   s6, s6, t0    # x15 ^= rot18(x11+x7)

    add   t0, t2, t5    # x0 + x3
    slli  t1, t0, 7     # x3 + x3 rot7
    srli  t0, t0, 25    # 
    or    t0, t0, t1    #
    xor   t3, t3, t0    # x1 ^= rot7(x0+x3)

    add   t0, t3, t2    # x1 + x0
    slli  t1, t0, 9     # x1 + x0 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   t4, t4, t0    # x2 ^= rot9(x1+x0)

    add   t0, t4, t3    # x2 + x1
    slli  t1, t0, 13    # x2 + x1 rot13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   t5, t5, t0    # x3 ^= rot13(x2+x1)

    add   t0, t5, t4    # x3 + x2
    slli  t1, t0, 18    # x3 + x2 rot18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   t2, t2, t0    # x0 ^= rot18(x3+x2)

    add   t0, a4, t6    # x5 + x4
    slli  t1, t0, 7     # x5 + x4 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   a5, a5, t0    # x6 ^= rot7(x5+x4)

    add   t0, a5, a4    # x6 + x5
    slli  t1, t0, 9     # x6 + x5 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   a6, a6, t0    # x7 ^= rot9(x6+x5)

    add   t0, a6, a5    # x7 + x6
    slli  t1, t0, 13    # x7 + x6 rot 13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   t6, t6, t0    # x4 ^= rot13(x7+x6)

    add   t0, t6, a6    # x4 + x7
    slli  t1, t0, 18    # x4 + x7 rot18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   a4, a4, t0    # x5 ^= rot18(x4+x7)

    add   t0, s1, s0    # x10 + x9
    slli  t1, t0, 7     # x10 + x9 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   s2, s2, t0    # x11 ^= rot7(x10+x9)

    add   t0, s2, s1    # x11 + x10
    slli  t1, t0, 9     # x11 + x10 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   a7, a7, t0    # x8 ^= rot9(x11+x10)

    add   t0, a7, s2    # x8 + x11
    slli  t1, t0, 13    # x8 + x11 rot13
    srli  t0, t0, 19    #
    or    t0, t0, t1    #
    xor   s0, s0, t0    # x9 ^= rot13(x8+x11)

    add   t0, s0, a7    # x9 + x8
    slli  t1, t0, 18    # x9 + x8 rot 18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   s1, s1, t0    # x10 ^= rot18(x9+x8)

    add   t0, s6, s5    # x15 + x14
    slli  t1, t0, 7     # x15 + x14 rot7
    srli  t0, t0, 25    #
    or    t0, t0, t1    #
    xor   s3, s3, t0    # x12 ^= rot7(x15+x14)

    add   t0, s3, s6    # x12 + x15
    slli  t1, t0, 9     # x12 + x15 rot9
    srli  t0, t0, 23    #
    or    t0, t0, t1    #
    xor   s4, s4, t0    # x13 ^= rot9(x12+x15)

    add   t0, s4, s3    # x13 + x12
    slli  t1, t0, 13    # x13 + x12 rot13
    srli  t0, t0, 19    # 
    or    t0, t0, t1    #
    xor   s5, s5, t0    # x14 ^= rot13(x13+x12)

    add   t0, s5, s4    # x14 + x13
    slli  t1, t0, 18    # x14 + x13 rot18
    srli  t0, t0, 14    #
    or    t0, t0, t1    #
    xor   s6, s6, t0    # x15 ^= rot18(x14+x13)

    addi  s7, s7, -2    # update loop counter
    bgtz  s7,  loop

    lwu    t0, 32(sp)    # j0
    lwu    t1, 36(sp)    # j1
    add   t2, t2, t0    # x0
    sw    t2, 0(a0)     # store x0

    lwu    t0, 40(sp)    # j2
    add   t3, t3, t1    # x1
    sw    t3, 4(a0)     # store x1

    lwu    t1, 44(sp)    # j3
    add   t4, t4, t0    # x2
    sw    t4, 8(a0)     # store x2
    
    lwu    t0, 48(sp)    # j4
    add   t5, t5, t1    # x3
    sw    t5, 12(a0)    # store x3
    
    lwu    t1, 52(sp)     # j5
    add   t6, t6, t0    # x4
    sw    t6, 16(a0)    # store x4

    lwu    t0, 56(sp)    # j6
    add   a4, a4, t1    # x5
    sw    a4, 20(a0)    # store x5

    lwu    t1, 60(sp)    # j7
    add   a5, a5, t0    # x6
    sw    a5, 24(a0)    # store x6

    lwu    t0, 64(sp)    # j8
    add   a6, a6, t1    # x7
    sw    a6, 28(a0)    # store x7

    lwu    t1, 68(sp)    # j9
    add   a7, a7, t0    # x8
    sw    a7, 32(a0)    # store x8

    lwu    t0, 72(sp)    # j10
    add   s0, s0, t1    # x9
    sw    s0, 36(a0)    # store x9

    lwu    t1, 76(sp)    # j11
    add   s1, s1, t0    # x10
    sw    s1, 40(a0)    # store x10

    lwu    t0, 80(sp)    # j12
    add   s2, s2, t1    # x11
    sw    s2, 44(a0)    # store x11

    lwu    t1, 84(sp)    # j13
    add   s3, s3, t0    # x12
    sw    s3, 48(a0)    # store x12

    lwu    t0, 88(sp)    # j14
    add   s4, s4, t1    # x13
    sw    s4, 52(a0)    # store x13

    lwu    t1, 92(sp)    # j15
    add   s5, s5, t0    # x14
    sw    s5, 56(a0)    # store x14

    add   s6, s6, t1    # x15
    sw    s6, 60(a0)    # store x15

    lwu    s0, 28(sp)    # restore variables
    lwu    s1, 24(sp)    #
    lwu    s2, 20(sp)    #
    lwu    s3, 16(sp)    #
    lwu    s4, 12(sp)    #
    lwu    s5, 8(sp)     #
    lwu    s6, 4(sp)     #
    lwu    s7, 0(sp)     #
    addi  sp, sp, 96    #

    ret


