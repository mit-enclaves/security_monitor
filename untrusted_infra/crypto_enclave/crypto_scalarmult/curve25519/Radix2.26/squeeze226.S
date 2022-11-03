.text

.globl squeeze226_255
.type squeeze226_255, @function

squeeze226_255:
    lwu    t0, 0(a0)     # load a
    lwu    t1, 4(a0)     #
    lwu    t2, 8(a0)     #
    lwu    t3, 12(a0)    #
    lwu    t4, 16(a0)    #
    lwu    t5, 20(a0)    #
    lwu    t6, 24(a0)    #
    lwu    a1, 28(a0)    #
    lwu    a2, 32(a0)    #
    lwu    a3, 36(a0)    # 

    srli  a4, t0, 26    # u = a[0]>>26
    li    a6, 0x3ffffff # 2^26-1
    and   t0, t0, a6    #
    add   a4, a4, t1    #
    and   t1, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t2    #
    and   t2, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t3    #
    and   t3, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t4    #
    and   t4, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t5    #
    and   t5, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t6    #
    and   t6, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, a1    #
    and   a1, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, a2    #
    and   a2, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, a3    #
    li    a5, 0x1fffff  # 2^21-1
    and   a3, a4, a5    #
    srli  a4, a4, 21    #

    slli  a5, a4, 4     # u*16
    slli  a7, a4, 1     # u*2
    add   a5, a5, a7    # u*18
    add   a4, a4, a5    # u*19

    add   a4, a4, t0    #
    and   t0, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t1    #
    and   t1, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t2    #
    and   t2, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t3    #
    and   t3, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t4    #
    and   t4, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t5    #
    and   t5, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, t6    #
    and   t6, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, a1    #
    and   a1, a4, a6    #
    srli  a4, a4, 26    #
    add   a4, a4, a2    #
    and   a2, a4, a6    #
    srli  a4, a4, 26    #
    add   a3, a4, a3    #

    sw    t0, 0(a0)     # store a
    sw    t1, 4(a0)     #
    sw    t2, 8(a0)     #
    sw    t3, 12(a0)    #
    sw    t4, 16(a0)    #
    sw    t5, 20(a0)    #
    sw    t6, 24(a0)    #
    sw    a1, 28(a0)    #
    sw    a2, 32(a0)    #
    sw    a3, 36(a0)    # 
    ret
