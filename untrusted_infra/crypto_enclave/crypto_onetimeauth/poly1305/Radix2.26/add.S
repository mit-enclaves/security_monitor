
.globl addasm
.type addasm , @function
addasm:
    li    t3, 0        # u = 0
    add   t5, a0, 64   # stopping condition h[16]
loop:
    lwu    t1, 0(a0)    # load h[0]
    lwu    t2, 0(a1)    # load c[0]
    add   t1, t1, t2   # t1= h[0]+c[0]
    add   t3, t3, t1   # u+= h[0]+c[0]
    andi   t2, t3, 255 # u & 255
    sw    t2, 0(a0)    # store u & 255 = h[0]
    srli  t3, t3, 8    # u >>= 8
    addi  a0, a0, 4    # update to h[i+1]
    addi  a1, a1, 4    # update to c[i+1]
    ble   a0, t5, loop
    ret

.globl add226asm
.type add226asm , @function
add226asm:
    addi  sp, sp, -16   # store variables
    sw    s0, 0(sp)     #
    sw    s1, 4(sp)     #
    sw    ra, 8(sp)     #

    mv    s0, a0        # store a0
    mv    s1, a1        # store a1

    li    t4, 0x3FFFFFF # load constant
    lwu    a0, 0(s0)     # load h[0]
    lwu    t2, 0(s1)     # load c[0]
    add   t3, a0, t2    # u= h[0]+c[0]
    and   a0, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a1, 4(s0)     # load h[1]
    lwu    t2, 4(s1)     # load c[1]
    add   a1, a1, t2    # t1= h[1]+c[1]
    add   t3, t3, a1    # u+= h[1]+c[1]
    and   a1, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a2, 8(s0)     # load h[2]
    lwu    t2, 8(s1)     # load c[0]
    add   a2, a2, t2    # t1= h[2]+c[2]
    add   t3, t3, a2    # u+= h[2]+c[2]
    and   a2, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a3, 12(s0)    # load h[3]
    lwu    t2, 12(s1)    # load c[3]
    add   a3, a3, t2    # t1= h[3]+c[3]
    add   t3, t3, a3    # u+= h[3]+c[3]
    and   a3, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a4, 16(s0)    # load h[4]
    lwu    t2, 16(s1)    # load c[4]
    add   a4, a4, t2    # t1= h[4]+c[4]
    add   a4, t3, a4    # u+= h[4]+c[4]

    call squeeze226param

    sw    a0, 0(s0)     #
    sw    a1, 4(s0)     #
    sw    a2, 8(s0)     #
    sw    a3, 12(s0)    #
    sw    a4, 16(s0)    #

    lwu    s0, 0(sp)     # restore variables
    lwu    s1, 4(sp)     #
    lwu    ra, 8(sp)     #
    add   sp, sp, 16
    ret

.globl add226asm_wo_squeeze
.type add226asm_wo_squeeze, @function
add226asm_wo_squeeze:
    addi  sp, sp, -16   # store variables
    sw    s0, 0(sp)     #
    sw    s1, 4(sp)     #

    mv    s0, a0        # store a0
    mv    s1, a1        # store a1

    li    t4, 0x3FFFFFF # load constant
    lwu    a0, 0(s0)     # load h[0]
    lwu    t2, 0(s1)     # load c[0]
    add   t3, a0, t2    # u= h[0]+c[0]
    and   a0, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a1, 4(s0)     # load h[1]
    lwu    t2, 4(s1)     # load c[1]
    add   a1, a1, t2    # t1= h[1]+c[1]
    add   t3, t3, a1    # u+= h[1]+c[1]
    and   a1, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a2, 8(s0)     # load h[2]
    lwu    t2, 8(s1)     # load c[0]
    add   a2, a2, t2    # t1= h[2]+c[2]
    add   t3, t3, a2    # u+= h[2]+c[2]
    and   a2, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a3, 12(s0)    # load h[3]
    lwu    t2, 12(s1)    # load c[3]
    add   a3, a3, t2    # t1= h[3]+c[3]
    add   t3, t3, a3    # u+= h[3]+c[3]
    and   a3, t3, t4    # u & 0x3FFFFFF

    srli  t3, t3, 26    # u >>= 26
    lwu    a4, 16(s0)    # load h[4]
    lwu    t2, 16(s1)    # load c[4]
    add   a4, a4, t2    # t1= h[4]+c[4]
    add   a4, t3, a4    # u+= h[4]+c[4]

    sw    a0, 0(s0)     #
    sw    a1, 4(s0)     #
    sw    a2, 8(s0)     #
    sw    a3, 12(s0)    #
    sw    a4, 16(s0)    #

    lwu    s0, 0(sp)     # restore variables
    lwu    s1, 4(sp)     #
    add   sp, sp, 16
    ret
