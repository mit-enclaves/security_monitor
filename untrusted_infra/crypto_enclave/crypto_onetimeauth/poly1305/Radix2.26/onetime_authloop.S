.text

.globl onetimeauth_loop
.type onetimeauth_loop, @function
onetimeauth_loop:
    addi   sp, sp, -32   # create space on stack
    sw     s0, 28(sp)    #
    sw     s1, 24(sp)    #
    sw     s2, 20(sp)    #
    sw     s3, 16(sp)    #
    sw     s4, 12(sp)    #
    sw     s5,  8(sp)    #
    sw     s6,  4(sp)    #
    sw     ra,  0(sp)    #
    mv     s0, a0        # in 
    mv     s1, a1        # inlen
    mv     s2, a2        # h
    mv     s3, a3        # r
    mv     s4, a4        # c
    li     s5, 0x3FFFFFF #
    li     s6, 16        # loop value

    blt    s1, s6, end_loop
loop:
    lbu    t2, 3(s0)     # in[3]
    lbu    t1, 2(s0)     # in[2]
    lbu    t0, 1(s0)     # in[1]
    andi   a0, t2, 3     # c[0] = in[3] & 3
    slli   a0, a0, 8     #
    or     a0, a0, t1    # c[0] << 2 +in[2]
    lbu    t1, 0(s0)     # in[0]
    slli   a0, a0, 8     #
    lw     t4, 0(s2)     # h[0] 
    or     a0, a0, t0    # c[0] << 8 + in[1]
    slli   a0, a0, 8     # 
    or     a0, a0, t1    # c[0] << 8 + in[0]
    add    t4, t4, a0    # c[0] + h[0]
    sw     t4, 0(s2)     # store h[0] += c[0]

    lbu    t3, 6(s0)     # in[6]
    lbu    t1, 5(s0)     # in[5]
    lbu    t0, 4(s0)     # in[4]
    andi   a0, t3, 15    # c[1] = in[6] & 15
    slli   a0, a0, 8     # c[1] << 4
    or     a0, a0, t1    # c[1] += in[5]
    slli   a0, a0, 8     # c[1] << 8
    or     a0, a0, t0    # c[1] += in[4]
    lw     t4, 4(s2)     # load h[1]
    slli   a0, a0, 6     # c[1] << 6 
    srli   t2, t2, 2     # in[3] >> 2
    or     a0, a0, t2    # c[1] += in[3]
    add    t4, t4, a0    #
    sw     t4, 4(s2)     # store h[1] += c[1]

    lbu    t2, 9(s0)     # in[9]
    lbu    t1, 8(s0)     # in[8]
    lbu    t0, 7(s0)     # in[7]
    andi   a0, t2, 63    # c[2] = in[9] & 63
    slli   a0, a0, 8     # c[2] <<= 2
    or     a0, a0, t1    # c[2] += in[8]
    slli   a0, a0, 8     # c[2] <<= 8
    lw     t4, 8(s2)     # load h[2]
    or     a0, a0, t0    # c[2] += in[7]
    slli   a0, a0, 4     # c[2] <<= 2 
    srli   t3, t3, 4     # in[6] >> 4
    or     a0, a0, t3    # c[2] += in[6] 
    add    t4, t4, a0    # 
    sw     t4, 8(s2)     # store h[2] += c[2]

    lbu    a0, 12(s0)    # in[12]
    lbu    t1, 11(s0)    # in[11]
    lbu    t0, 10(s0)    # in[10]
    slli   a0, a0, 8     # c[3] <<= 8
    or     a0, a0, t1    # c[3] += in[11]
    slli   a0, a0, 8     # c[3] <<= 8
    or     a0, a0, t0    # c[3] += in[10]
    lw     t4, 12(s2)    # load h[3]
    slli   a0, a0, 2     # c[3] <<= 2
    srli   t2, t2, 6     # in[9] >> 6
    or     a0, a0, t2    # c[3] += in[9]
    add    t4, t4, a0    #
    sw     t4, 12(s2)    # store h[3] += c[3]

    lbu    a0, 15(s0)    # in[15]
    lbu    t1, 14(s0)    # in[14]
    lbu    t2, 13(s0)    # in[13]
    addi   a0, a0, 0x100 # append the 1 bit at the end
    slli   a0, a0, 8     # c[4] <<= 8
    lw     t4, 16(s2)    # load h[4]
    or     a0, a0, t1    # c[4] += in[14]
    slli   a0, a0, 8     # c[4] <<= 8
    or     a0, a0, t2    # c[4] += in[13]
    add    t4, t4, a0    #
    sw     t4, 16(s2)    # store h[4] += c[4]

    mv     a0, s2        #
    mv     a1, s3        #
    call   mulmod226asm

    addi   s0, s0, 16    #
    addi   s1, s1, -16   # update loop variables
    bge    s1, s6, loop
    
end_loop:
    mv     a0, s1        # return inlen
    lw     s0, 28(sp)    # restore values
    lw     s1, 24(sp)    #
    lw     s2, 20(sp)    #
    lw     s3, 16(sp)    #
    lw     s4, 12(sp)    #
    lw     s5,  8(sp)    #
    lw     s6,  4(sp)    #
    lw     ra,  0(sp)    #
    addi   sp, sp, 32    
    ret
