.text

# A0 p, A1 q, A2 r, A3 s, A4 b
.globl crypto_select_asm
.type crypto_select_asm, @function
crypto_select_asm:
    lwu    t0, 0(a2)     # r[0]
    lwu    t1, 0(a3)     # s[0]
    addi  a4, a4, -1    # mask of 2^32-1 if b==0 if b==1 0 mask
    xor   t2, t0, t1    # r[0]^s[0]
    and   t2, a4, t2    # mask & (r[0]^s[0])
    lwu    t3, 4(a2)     # r[1]
    lwu    t4, 4(a3)     # s[1]
    xor   t5, t1, t2    # r[0] if mask==0 otherwise s[0]
    xor   t6, t0, t2    # s[0] if mask==0 otherwise r[0]
    sw    t5, 0(a0)     # store res in p[0]
    sw    t6, 0(a1)     # store res in q[0]

    xor   t2, t3, t4    # r[1]^s[1]
    and   t2, t2, a4    #
    lwu    t0, 8(a2)     # r[2]
    lwu    t1, 8(a3)     # s[2]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 4(a0)     #
    sw    t6, 4(a1)     #

    xor   t2, t0, t1    # r[2]^s[2]
    and   t2, t2, a4    #
    lwu    t3, 12(a2)    # r[3]
    lwu    t4, 12(a3)    # s[3]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 8(a0)     #
    sw    t6, 8(a1)     #

    xor   t2, t3, t4    # r[3]^s[3]
    and   t2, t2, a4    #
    lwu    t0, 16(a2)    # r[4]
    lwu    t1, 16(a3)    # s[4]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 12(a0)    #
    sw    t6, 12(a1)    #

    xor   t2, t0, t1    # r[4]^s[4]
    and   t2, t2, a4    #
    lwu    t3, 20(a2)    # r[5]
    lwu    t4, 20(a3)    # s[5]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 16(a0)    #
    sw    t6, 16(a1)    #

    xor   t2, t3, t4    # r[5]^s[5]
    and   t2, t2, a4    #
    lwu    t0, 24(a2)    # r[6]
    lwu    t1, 24(a3)    # s[6]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 20(a0)    #
    sw    t6, 20(a1)    #

    xor   t2, t0, t1    # r[6]^s[6]
    and   t2, t2, a4    #
    lwu    t3, 28(a2)    # r[7]
    lwu    t4, 28(a3)    # s[7]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 24(a0)    #
    sw    t6, 24(a1)    #

    xor   t2, t3, t4    # r[7]^s[7]
    and   t2, t2, a4    #
    lwu    t0, 32(a2)    # r[8]
    lwu    t1, 32(a3)    # s[8]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 28(a0)    #
    sw    t6, 28(a1)    #

    xor   t2, t0, t1    # r[8]^s[8]
    and   t2, t2, a4    #
    lwu    t3, 36(a2)    # r[9]
    lwu    t4, 36(a3)    # s[9]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 32(a0)    #
    sw    t6, 32(a1)    #

    xor   t2, t3, t4    # r[9]^s[9]
    and   t2, t2, a4    #
    lwu    t0, 40(a2)    # r[10]
    lwu    t1, 40(a3)    # s[10]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 36(a0)    #
    sw    t6, 36(a1)    #

    xor   t2, t0, t1    # r[10]^s[10]
    and   t2, t2, a4    #
    lwu    t3, 44(a2)    # r[11]
    lwu    t4, 44(a3)    # s[11]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 40(a0)    #
    sw    t6, 40(a1)    #

    xor   t2, t3, t4    # r[11]^s[11]
    and   t2, t2, a4    #
    lwu    t0, 48(a2)    # r[12]
    lwu    t1, 48(a3)    # s[12]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 44(a0)    #
    sw    t6, 44(a1)    #

    xor   t2, t0, t1    # r[12]^s[12]
    and   t2, t2, a4    #
    lwu    t3, 52(a2)    # r[13]
    lwu    t4, 52(a3)    # s[13]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 48(a0)    #
    sw    t6, 48(a1)    #

    xor   t2, t3, t4    # r[13]^s[13]
    and   t2, t2, a4    #
    lwu    t0, 56(a2)    # r[14]
    lwu    t1, 56(a3)    # s[14]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 52(a0)    #
    sw    t6, 52(a1)    #

    xor   t2, t0, t1    # r[14]^s[14]
    and   t2, t2, a4    #
    lwu    t3, 60(a2)    # r[15]
    lwu    t4, 60(a3)    # s[15]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 56(a0)    #
    sw    t6, 56(a1)    #

    xor   t2, t3, t4    # r[15]^s[15]
    and   t2, t2, a4    #
    lwu    t0, 64(a2)    # r[16]
    lwu    t1, 64(a3)    # s[16]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 60(a0)    #
    sw    t6, 60(a1)    #

    xor   t2, t0, t1    # r[16]^s[16]
    and   t2, t2, a4    #
    lwu    t3, 68(a2)    # r[17]
    lwu    t4, 68(a3)    # s[17]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 64(a0)    #
    sw    t6, 64(a1)    #

    xor   t2, t3, t4    # r[17]^s[17]
    and   t2, t2, a4    #
    lwu    t0, 72(a2)    # r[18]
    lwu    t1, 72(a3)    # s[18]
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 68(a0)    #
    sw    t6, 68(a1)    #

    xor   t2, t0, t1    # r[18]^s[18]
    and   t2, t2, a4    #
    lwu    t3, 76(a2)    # r[19]
    lwu    t4, 76(a3)    # s[19]
    xor   t5, t1, t2    #
    xor   t6, t0, t2    #
    sw    t5, 72(a0)    #
    sw    t6, 72(a1)    #

    xor   t2, t3, t4    # r[19]^s[19]
    and   t2, t2, a4    #
    xor   t5, t4, t2    #
    xor   t6, t3, t2    #
    sw    t5, 76(a0)    #
    sw    t6, 76(a1)    #
    
    ret
