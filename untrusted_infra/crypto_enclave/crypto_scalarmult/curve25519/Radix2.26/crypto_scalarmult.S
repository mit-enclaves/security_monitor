.text

.globl crypto_scalarmult_asm
.type crypto_scalarmult_asm, @function
crypto_scalarmult_asm:
    addi  sp, sp, -144   # create stack space
    sw    ra, 140(sp)    # 
    sw    s0, 136(sp)    # q address
    sw    s2, 132(sp)    # p address

    mv    s0, a0         #
    mv    s2, a2         #

    mv    a0, sp         # e226
    call  convert_to_radix226_255

    lwu    t0, 0(sp)      # e226[0]
    lwu    t1, 36(sp)     # e226[9]
    li    t2, 0x3fffff8  # 2^26-8
    and   t0, t0, t2     # clear 3 least significant bits
    sw    t0, 0(sp)      # store e226[0]
    li    t2, 0x1fffff   # 2^21-1
    and   t1, t1, t2     # cut e226 to 255 bits
    li    t2, 0x100000   # 2^20
    or    t1, t1, t2     # set the 255 bit to 1
    sw    t1, 36(sp)     #

    addi  a0, sp, 40     # work226 address
    mv    a1, s2         # 
    call convert_to_radix226_255

    addi  a0, sp, 40     # work226
    addi  a1, sp, 0      # e226
    call mainloop226_asm #

    addi  a0, sp, 80     # work226[10]
    addi  a1, sp, 80     # work226[10]
    call recip226
    
    addi  a0, sp, 40     # work226
    addi  a1, sp, 40     # work226
    addi  a2, sp, 80     # work226[10]
    call karatsuba226_255

    addi  a0, sp, 40     # work226
    call freeze

    mv    a0, s0         # q
    addi  a1, sp, 40     # work226
    call toradix28_255

    lwu    ra, 140(sp)    # restore variables
    lwu    s0, 136(sp)    #
    lwu    s2, 132(sp)    #
    addi  sp, sp, 144    #

    ret
