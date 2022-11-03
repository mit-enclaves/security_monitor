.text

.globl squeeze226asm
.type squeeze226asm,@function
squeeze226asm:
    mv    a5, a0          # store address
    mv    a6, ra          #
    lwu    a0, 0(a5)       #
    lwu    a1, 4(a5)       # in[1]
    lwu    a2, 8(a5)       # in[2]
    lwu    a3, 12(a5)      # in[3]
    lwu    a4, 16(a5)      # in[4]

    call  squeeze226param
    
    sw    a0, 0(a5)       # store result
    sw    a1, 4(a5)       #
    sw    a2, 8(a5)       #
    sw    a3, 12(a5)      #
    sw    a4, 16(a5)      #
    mv    ra, a6

    ret


.globl squeeze226param
.type squeeze226param, @function
squeeze226param:
    li    t5, 0x3ffffff   # 2^26-1
    srli  t0, a4, 26      #
    slli  t1, t0, 2       # overflow result[4]*4
    add   t0, t0, t1      # overflow result[4]*5
    add   a0, a0, t0      #
    and   a4, a4, t5      #
    srli  t0, a0, 26      # overflow result[0]
    add   a1, a1, t0      #
    and   a0, a0, t5      #
    srli  t0, a1, 26      # overflow result[1]
    add   a2, a2, t0      #
    and   a1, a1, t5      #
    srli  t0, a2, 26      # overflow result[2]
    add   a3, a3, t0      #
    and   a2, a2, t5      #
    srli  t0, a3, 26      # overflow result[3]
    add   a4, a4, t0      #
    and   a3, a3, t5      #
    srli  t0, a4, 26      # overflow result[4]
    slli  t1, t0, 2       # 
    add   t0, t1, t0      #
    add   a0, a0, t0      #
    and   a4, a4, t5      #
    ret

