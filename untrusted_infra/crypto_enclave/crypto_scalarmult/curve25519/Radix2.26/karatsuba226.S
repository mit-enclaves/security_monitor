.text

.globl karatsuba226_255
.type karatsuba226_255, @function
karatsuba226_255:

    addi  sp, sp, -144  # store variables on stack
    sw    s0, 140(sp)   #
    sw    s1, 136(sp)   #
    sw    s2, 132(sp)   #
    sw    s3, 128(sp)   #
    sw    s4, 124(sp)   #
    sw    ra, 120(sp)   #

    mv    s0, a0        # result address
    mv    s1, a1        # A address
    mv    s2, a2        # B address
    li    s4, 0x3ffffff # 2^26-1

    addi  a0, sp, 40    # result x
    call karatsuba226_255_5asm # A0..A4 *B0..B4 = x

    lwu    t0, 0(s1)     # A0
    lwu    t5, 20(s1)    # A5
    lwu    t1, 4(s1)     # A2
    li    a6, 0x4000000 # 2^26
    sub   t0, t0, t5    # A0-A5
    srai  a0, t0, 31    # sign of t0
    and   a1, a6, a0    # mask 2^26 with the sign
    lwu    t6, 24(s1)    # A6
    add   t0, t0, a1    # apply borrow
    andi  a0, a0, 1     # borrow from A1
    sub   t1, t1, a0    #
    sub   t1, t1, t6    # A1-A6
    lwu    t2, 8(s1)     # A2
    srai  a0, t1, 31    # sign of A1
    and   a1, a6, a0    # mask 2^26 with the sign
    lwu    t5, 28(s1)    # A7
    add   t1, t1, a1    # apply borrow
    andi  a0, a0, 1     # borrow from A2
    sub   t2, t2, a0    #
    sub   t2, t2, t5    #
    lwu    t3, 12(s1)    # A3
    srai  a0, t2, 31    # sign of A2
    and   a1, a6, a0    # mask 2^26 with sign
    lwu    t6, 32(s1)    # A8
    add   t2, a1, t2    # apply borrow
    andi  a0, a0, 1     # borrow from A3
    sub   t3, t3, a0    #
    sub   t3, t3, t6    # A3-A8
    lwu    t4, 16(s1)    # A4
    srai  a0, t3, 31    # sign of A3
    and   a1, a6, a0    # mask 2^26 with sign
    lwu    t5, 36(s1)    # A9
    add   t3, a1, t3    # apply borrow
    andi  a0, a0, 1     # borrow from A4
    sub   t4, t4, a0    #
    sub   t4, t4, t5    # A4-A9
    srai  s3, t4, 31    # sign of A0..A4-A5..A9
    and   a0, s3, s4    # mask 2^26-1 with the sign
    xor   t0, t0, a0    # absolute value of A0..A4-A5..A9
    xor   t1, t1, a0    #
    xor   t2, t2, a0    #
    xor   t3, t3, a0    #
    xor   t4, t4, s3    #
    neg   a0, s3        #
    add   t0, t0, a0    # 

    sw    t0, 80(sp)    # store |A0..A4-A5..A9|
    sw    t1, 84(sp)    #
    sw    t2, 88(sp)    #
    sw    t3, 92(sp)    #
    sw    t4, 96(sp)    #

    lwu    t0, 0(s2)     # B0
    lwu    t5, 20(s2)    # B5
    lwu    t1, 4(s2)     # B2
    sub   t0, t0, t5    # B0-B5
    srai  a0, t0, 31    # sign of t0
    and   a1, a6, a0    # mask 2^26 with the sign
    lwu    t6, 24(s2)    # B6
    add   t0, t0, a1    # apply borrow
    andi  a0, a0, 1     # borrow from B1
    sub   t1, t1, a0    #
    sub   t1, t1, t6    # B1-B6
    lwu    t2, 8(s2)     # B2
    srai  a0, t1, 31    # sign of B1
    and   a1, a6, a0    # mask 2^26 with the sign
    lwu    t5, 28(s2)    # B7
    add   t1, t1, a1    # apply borrow
    andi  a0, a0, 1     # borrow from B2
    sub   t2, t2, a0    #
    sub   t2, t2, t5    # B2-B7
    lwu    t3, 12(s2)    # B3
    srai  a0, t2, 31    # sign of B2
    and   a1, a6, a0    # mask 2^26 with sign
    lwu    t6, 32(s2)    # B8
    add   t2, t2, a1    # apply borrow
    andi  a0, a0, 1     # borrow from B3
    sub   t3, t3, a0    #
    sub   t3, t3, t6    # B3-B8
    lwu    t4, 16(s2)    # B4
    srai  a0, t3, 31    # sign of B3
    and   a1, a6, a0    # mask 2^26 with sign
    lwu    t5, 36(s2)    # B9
    add   t3, a1, t3    # apply borrow
    andi  a0, a0, 1     # borrow from B4
    sub   t4, t4, a0    #
    sub   t4, t4, t5    # B4-B9
    srai  a1, t4, 31    # sign of B0..B4-B5..B9
    and   a0, a1, s4    # mask 2^26-1 with the sign
    xor   t0, t0, a0    # absolute value of B0..B4-B5..B9
    xor   t1, t1, a0    #
    xor   t2, t2, a0    #
    xor   t3, t3, a0    #
    xor   t4, t4, a1    #
    neg   a0, a1        #
    add   t0, t0, a0    # 
    xor   s3, s3, a1    # sign of the multiplication
    sw    t0, 100(sp)   # store |B0..B4-B5..B9|
    sw    t1, 104(sp)   #
    sw    t2, 108(sp)   #
    sw    t3, 112(sp)   #
    sw    t4, 116(sp)   #

    mv    a0, sp        # result address
    addi  a1, sp, 80    # address |A0..A4-A5..A9|
    addi  a2, sp, 100   # address |B0..B4-B5..B9| 
    call   karatsuba226_255_5asm # multiplication of a1 and a2=m

    addi  a0, sp, 80    # result address
    addi  a1, s1, 20    # address A5..A9
    addi  a2, s2, 20    # address B5..B9
    call   karatsuba226_255_5asm # (A5..A9)*(B5..B9) = y

    lwu    t4, 60(sp)    # x_5
    lwu    t5, 64(sp)    # x_6
    lwu    t6, 68(sp)    # x_7
    lwu    a0, 72(sp)    # x_8
    lwu    a1, 76(sp)    # x_9

    lwu    a2, 80(sp)    # y_0
    lwu    a3, 84(sp)    # y_1
    lwu    a4, 88(sp)    # y_2
    lwu    a5, 92(sp)    # y_3
    lwu    a6, 96(sp)    # y_4

    add   a2, a2, t4    # res[5] = x_5+y_0
    lwu    t0, 100(sp)   # y_5
    add   a3, a3, t5    # res[6] = x_6+y_1
    add   a4, a4, t6    # res[7] = x_7+y_2
    add   a5, a5, a0    # res[8] = x_8+y_3
    add   a6, a6, a1    # res[9] = x_9+y_4

    lwu    t3, 104(sp)   # y_6
    add   t4, a2, t0    # res[0] = x_5+y_0+y_5
    slli  t1, t0, 1     # y_5*2
    slli  t2, t0, 4     # y_5*16
    add   t1, t1, t2    # y_5*18
    add   t0, t0, t1    # y_5*19
    srli  s1, t0, 21    # y_5*608 with overflow carrying
    slli  t0, t0, 5     # y_5*608
    and   t0, t0, s4    # y_5*608_l
    add   a2, a2, t0    # res[5] += 608*y_5

    lwu    t0, 108(sp)   # y_7
    add   t5, a3, t3    # res[1] = x_5+y_1+y_6
    slli  t1, t3, 1     # y_6*2
    slli  t2, t3, 4     # y_6*16
    add   t1, t1, t2    # y_6*18
    add   t3, t3, t1    # y_6*19
    srli  s2, t3, 21    # y_6*608 overflow
    slli  t3, t3, 5     # y_6*608
    and   t3, t3, s4    #
    add   t3, t3, s1    # carry from y_5*608
    add   a3, a3, t3    # res[6] += 608*y_6

    lwu    t3, 112(sp)   # y_8
    add   t6, a4, t0    # res[2] = x_6+y_2+y_7
    slli  t1, t0, 1     # y_7*2
    slli  t2, t0, 4     # y_7*16
    add   t1, t1, t2    # y_7*18
    add   t1, t1, t0    # y_7*19
    srli  s1, t1, 21    # y_7*608 overflow
    slli  t1, t1, 5     # y_7*608
    and   t1, t1, s4    #
    add   t1, t1, s2    # carry from y_6*608
    add   a4, a4, t1    # res[7] += 608*y_7
    
    lwu    t0, 116(sp)   # y_9
    add   a0, a5, t3    # res[3] = x_7+y_3+y_8
    slli  t1, t3, 1     # y_8*2
    slli  t2, t3, 4     # y_8*16
    add   t1, t1, t2    # y_8*18
    add   t1, t1, t3    # y_8*19
    srli  s2, t1, 21    # y_8*608 overflow
    slli  t1, t1, 5     # y_8*608
    and   t1, t1, s4    #
    add   t1, t1, s1    # carry from y_7*608
    add   a5, a5, t1    # res[8] += 608*y_8

    lwu    t3, 20(sp)    # m_5
    add   a1, a6, t0    # res[4] = x_8+y_4+y_9
    slli  t1, t0, 1     # y_9*2
    slli  t2, t0, 4     # y_9*16
    add   t1, t1, t2    # y_9*18
    add   t1, t1, t0    # y_9*19
    srli  s1, t1, 21    # y_9*608 overflow
    slli  t1, t1, 5     # y_9*608
    and   t1, t1, s4    #
    add   t1, t1, s2    # y_8*608 overflow
    add   a6, a6, t1    # res[9] += 608*y_9
    slli  t1, s1, 1     # y_9*608 overflow *2
    slli  t2, s1, 4     # y_9*608 overflow *16
    add   t1, t1, t2    # y_9*608 overflow *19
    slli  t1, t1, 5     # y_9*608 overflow *608
    add   t4, t4, t1    # res[0] += 608*overflow

    lwu    t0, 24(sp)    # m_6
    add   t3, t3, s3    # negative if negflag is set otherwise leave as is
    xor   t3, t3, s3    #
    sub   t4, t4, t3    # res[0] -= m_5
    sltz  t1, t4        # handle underflow
    sub   t5, t5, t1    # borrow from res[1]
    li    a7, 0x4000000 # 2^26
    neg   t1, t1        # create mask
    and   t1, a7, t1    # 2^26 masked
    add   t4, t4, t1    #
    srli  t1, t4, 26    # handle overflow
    add   t5, t5, t1    #
    and   t4, t4, s4    #
     
    lwu    t3, 28(sp)    # m_7
    add   t0, t0, s3    # negative if negflag is set other leave as is
    xor   t0, t0, s3    #
    sub   t5, t5, t0    # res[1] -= m_6
    sltz  t1, t5        # handle underflow
    sub   t6, t6, t1    # borrow from res[2]
    neg   t1, t1        # create mask
    and   t1, a7, t1    # 2^26 masked
    add   t5, t5, t1    #
    srli  t1, t5, 26    # handle overflow
    add   t6, t6, t1    #
    and   t5, t5, s4    #

    lwu    t0, 32(sp)    # m_8
    add   t3, t3, s3    # negative if negflag is set otherwise leave as is
    xor   t3, t3, s3    #
    sub   t6, t6, t3    # res[2] -= m_7
    sltz  t1, t6        # handle underflow
    sub   a0, a0, t1    # borrow from res[3]
    neg   t1, t1        # create mask
    and   t1, a7, t1    # 2^26 masked
    add   t6, t6, t1    #
    srli  t1, t6, 26    # handle overflow
    add   a0, a0, t1    #
    and   t6, t6, s4    #

    lwu    t3, 36(sp)    # m_9
    add   t0, t0, s3    # negative if negflag is set otherwise leave as is
    xor   t0, t0, s3    #
    sub   a0, a0, t0    # res[3] -= m_8
    sltz  t1, a0        # handle underflow
    sub   a1, a1, t1    # borrow from res[4]
    neg   t1, t1        # create mask
    and   t1, t1, a7    # 2^26 masked
    add   a0, a0, t1    # 
    srli  t1, a0, 26    # handle overflow
    add   a1, a1, t1    #
    and   a0, a0, s4    #

    add   t3, t3, s3    # negative if negflag is set otherwise leave as is
    xor   t3, t3, s3    #
    sub   a1, a1, t3    # res[4] -= m_9
    sltz  t1, a0        # handle underflow
    li    t2, 608       # 
    neg   t1, t1        # create mask
    and   t2, t2, t1    # 608 masked
    sub   a2, a2, t2    # borrow 608 from res[5]
    and   t1, t1, a7    # 2^26 masked
    add   a1, a1, t1    #
    srli  t1, a1, 26    # handle overflow
    slli  t2, t1, 1     # overflow *2
    slli  t3, t1, 4     # overflow*16
    add   t2, t2, t3    # overflow*18
    add   t1, t1, t2    # overflow*19
    slli  t1, t1, 5     # overflow*608
    add   a2, a2, t1    #
    and   a1, a1, s4    #

    slli  t1, t4, 1     # res[0]*2
    slli  t2, t4, 4     # res[0]*16
    add   t1, t1, t2    # res[0]*18
    add   t4, t4, t1    # res[0]*19
    srli  s1, t4, 21    # res[0]*608 overflow
    slli  t4, t4, 5     # res[0]*608_l
    and   t4, t4, s4    # 

    slli  t1, t5, 1     # res[1]*2
    slli  t2, t5, 4     # res[1]*16
    add   t1, t1, t2    # res[1]*18
    add   t5, t5, t1    # res[1]*19
    srli  s2, t5, 21    # res[1]*608 overflow
    slli  t5, t5, 5     # res[1]*608_l
    and   t5, t5, s4    #
    add   t5, t5, s1    # res[0]*608 overflow
    
    slli  t1, t6, 1     # res[2]*2
    slli  t2, t6, 4     # res[2]*16
    add   t1, t1, t2    # res[2]*18
    add   t6, t1, t6    # res[2]*19
    srli  s1, t6, 21    # res[2]*608 overflow
    slli  t6, t6, 5     # res[2]*608_l
    and   t6, t6, s4    #
    add   t6, t6, s2    # res[1]*608 overflow

    slli  t1, a0, 1     # res[3]*2
    slli  t2, a0, 4     # res[3]*16
    add   t1, t1, t2    # res[3]*18
    add   a0, t1, a0    # res[3]*19
    srli  s2, a0, 21    # res[3]*608 overflow
    slli  a0, a0, 5     # res[3]*608_l
    and   a0, a0, s4    #
    add   a0, a0, s1    # res[2]*608 overflow

    slli  t1, a1, 1     # res[4]*2
    slli  t2, a1, 4     # res[4]*16
    add   t1, t1, t2    # res[4]*18
    add   a1, t1, a1    # res[4]*19
    srli  s1, a1, 21    # res[4]*608 overflow
    slli  a1, a1, 5     # res[4]*608_l
    and   a1, a1, s4    #
    add   a1, a1, s2    # res[3]*608 overflow
    add   a2, a2, s1    # res[5] += res[4]*608 overflow

    lwu    t0, 40(sp)    # x_0
    lwu    t1, 44(sp)    # x_1
    lwu    t2, 48(sp)    # x_2
    lwu    s2, 52(sp)    # x_3
    lwu    t3, 56(sp)    # x_4
    add   a2, a2, t0    # res[5] += x_0
    add   t4, t4, t0    # res[0] += x_0
    lwu    t0, 0(sp)     # m_0
    add   a3, a3, t1    # res[6] += x_1
    add   t5, t5, t1    # res[1] += x_1
    add   a4, a4, t2    # res[7] += x_2
    add   t6, t6, t2    # res[2] += x_2
    add   a5, a5, s2    # res[8] += x_3
    add   a0, a0, s2    # res[3] += x_3
    add   a6, a6, t3    # res[9] += x_4
    add   a1, a1, t3    # res[4] += x_4

    lwu    t3, 4(sp)     # m_1
    add   t0, t0, s3    # negative if negflag otherwise leave as is
    xor   t0, t0, s3    #
    sub   a2, a2, t0    # res[5] -= m_0
    sltz  t1, a2        # handle underflow
    sub   a3, a3, t1    # borrow from res[6]
    neg   t1, t1        # create mask
    and   t1, t1, a7    # 2^26 masked
    add   a2, a2, t1    #

    lwu    t0, 8(sp)     # m_2
    add   t3, t3, s3    # negative if negflag otherwise leave as is
    xor   t3, t3, s3    #
    sub   a3, a3, t3    # res[6] -= m_1
    sltz  t1, a3        # handle underflow
    sub   a4, a4, t1    # borrow from res[7]
    neg   t1, t1        # create mask
    and   t1, t1, a7    # 2^26 masked
    add   a3, a3, t1    #

    lwu    t3, 12(sp)    # m_3
    add   t0, t0, s3    # negative if negflag otherwise leave as is
    xor   t0, t0, s3    #
    sub   a4, a4, t0    # res[7] -= m_2
    sltz  t1, a4        # handle underflow
    sub   a5, a5, t1    # borrow from res[8]
    neg   t1, t1        # create mask
    and   t1, t1, a7    # 2^26 masked
    add   a4, a4, t1    #

    lwu    t0, 16(sp)    # m_4
    add   t3, t3, s3    # negative if negflag otherwise leave as is
    xor   t3, t3, s3    #
    sub   a5, a5, t3    # res[8] -= m_3
    sltz  t1, a5        # handle underflow
    sub   a6, a6, t1    # borrow from res[9]
    neg   t1, t1        # create mask
    and   t1, t1, a7    # 2^26 masked
    add   a5, a5, t1    #

    add   t0, t0, s3    # negative if negflag otherwise leave as is
    xor   t0, t0, s3    #
    sub   a6, a6, t0    # res[9] -= m_4
    sltz  t1, a6        # handle underflow
    neg   t1, t1        # create mask
    li    t2, 608       #
    and   t2, t1, t2    #
    sub   t4, t4, t2    # borrow 608 from res[0]
    and   t1, t1, a7    # 2^26 masked
    add   a6, a6, t1    #

    srli  t0, t4, 26    # overflow in res[0]
    add   t5, t5, t0    #
    and   t4, t4, s4    #
    srli  t0, t5, 26    # overflow in res[1]
    add   t6, t6, t0    #
    and   t5, t5, s4    #
    srli  t0, t6, 26    # overflow in res[2]
    add   a0, a0, t0    #
    and   t6, t6, s4    #
    srli  t0, a0, 26    # overflow in res[3]
    add   a1, a1, t0    #
    and   a0, a0, s4    #
    srli  t0, a1, 26    # overflow in res[4]
    add   a2, a2, t0    #
    and   a1, a1, s4    #
    srli  t0, a2, 26    # overflow in res[5]
    add   a3, a3, t0    #
    and   a2, a2, s4    #
    srli  t0, a3, 26    # overflow in res[6]
    add   a4, a4, t0    #
    and   a3, a3, s4    #
    srli  t0, a4, 26    # overflow in res[7]
    add   a5, a5, t0    #
    and   a4, a4, s4    #
    srli  t0, a5, 26    # overflow in res[8]
    add   a6, a6, t0    #
    and   a5, a5, s4    #
    srli  t0, a6, 21    # overflow in res[9]
    slli  t1, t0, 1     # overflow *2
    slli  t2, t0, 4     # overflow *16
    add   t1, t1, t2    # overflow *18
    add   t1, t1, t0    # overflow*19
    li    t0, 0x1fffff  # 2^21
    and   a6, a6, t0    # 
    add   t4, t4, t1    #

    sw    t4, 0(s0)     # store result
    sw    t5, 4(s0)     #
    sw    t6, 8(s0)     #
    sw    a0, 12(s0)    #
    sw    a1, 16(s0)    #
    sw    a2, 20(s0)    #
    sw    a3, 24(s0)    #
    sw    a4, 28(s0)    #
    sw    a5, 32(s0)    #
    sw    a6, 36(s0)    #

    lwu    s0, 140(sp)   # restore variables
    lwu    s1, 136(sp)   #
    lwu    s2, 132(sp)   #
    lwu    s3, 128(sp)   #
    lwu    s4, 124(sp)   #
    lwu    ra, 120(sp)   #
    addi  sp, sp, 144   #
    ret
