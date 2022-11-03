.text

.globl mainloop226_asm
.type mainloop226_asm, @function
mainloop226_asm:
    addi  sp, sp, -432 # create stack space
    sw    s0, 428(sp)  # store variables
    sw    s1, 424(sp)  # a1
    sw    s2, 420(sp)  # i
    sw    s3, 416(sp)  # j
    sw    s4, 412(sp)  # b
    sw    s5, 408(sp)  # 
    sw    ra, 404(sp)  # 

    mv    s0, a0        # store work pointer
    mv    s1, a1        # store e pointer

    lwu    t0, 0(s0)     # preload work[0]
    lwu    t1, 4(s0)     # preload work[1]

    lwu    t2, 8(s0)     # work[2]
    lwu    t3, 12(s0)    # work[3]
    sw    t0, 0(sp)     # xzm1[0]=work[0]
    sw    t1, 4(sp)     # xzm1[1]
    lwu    t0, 16(s0)    # work[4]
    lwu    t1, 20(s0)    # work[5]
    sw    t2, 8(sp)     # xzm1[2]
    sw    t3, 12(sp)    # xzm1[3]
    lwu    t2, 24(s0)    # work[6]
    lwu    t3, 28(s0)    # work[7]
    sw    t0, 16(sp)    # xzm1[4]
    sw    t1, 20(sp)    # xzm1[5]
    lwu    t0, 32(s0)    # work[8]
    lwu    t1, 36(s0)    # work[9]
    sw    t2, 24(sp)    # xzm1[6]
    sw    t3, 28(sp)    # xzm1[7]
    sw    t0, 32(sp)    # xzm1[8]
    sw    t1, 36(sp)    # xzm1[9]
    li    t0, 1         #
    sw    t0, 40(sp)    # xzm1[10]=1
    sw    t0, 80(sp)    # xzm[0]=1
    sw    zero, 44(sp)  # xzm1[11]=0
    sw    zero, 48(sp)  # xzm1[12]=0
    sw    zero, 52(sp)  # xzm1[13]=0
    sw    zero, 56(sp)  # xzm1[14]=0
    sw    zero, 60(sp)  # xzm1[15]=0
    sw    zero, 64(sp)  # xzm1[16]=0
    sw    zero, 68(sp)  # xzm1[17]=0
    sw    zero, 72(sp)  # xzm1[18]=0
    sw    zero, 76(sp)  # xzm1[19]=0

    sw    zero, 84(sp)  # xzm[1..19]=0
    sw    zero, 88(sp)  #
    sw    zero, 92(sp)  #
    sw    zero, 96(sp)  #
    sw    zero, 100(sp) #
    sw    zero, 104(sp) #
    sw    zero, 108(sp) #
    sw    zero, 112(sp) #
    sw    zero, 116(sp) #
    sw    zero, 120(sp) #
    sw    zero, 124(sp) #
    sw    zero, 128(sp) #
    sw    zero, 132(sp) #
    sw    zero, 136(sp) #
    sw    zero, 140(sp) #
    sw    zero, 144(sp) #
    sw    zero, 148(sp) #
    sw    zero, 152(sp) #
    sw    zero, 156(sp) #

    lwu    s5, 36(s1)    # e[9]
    li    s3, 20        # loop variable

firstloop:
    srl   s4, s5, s3    # e[9] >> loop variable
    andi  s4, s4, 1     # bit 234+j

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 80    # xzm
    addi  a3, sp, 0     # xzm1
    mv    a4, s4        # b
    call  crypto_select_asm

    addi  a0, sp, 160   # a0
    addi  a1, sp, 80    # xzm
    addi  a2, sp, 120   # xzm[10]
    call  add226

    addi  a0, sp, 200   # a0[10]
    addi  a1, sp, 80    # xzm
    addi  a2, sp, 120   # xzm[10]
    call sub226

    addi  a0, sp, 240   # a1
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 40    # xzm1[10]
    call add226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 40    # xzm1[10]
    call sub226

    addi  a0, sp, 320   # b0
    addi  a1, sp, 160   # a0
    addi  a2, sp, 160   # a0
    call karatsuba226_255

    addi  a0, sp, 360   # b0[10]
    addi  a1, sp, 200   # a0[10]
    addi  a2, sp, 200   # a0[10]
    call karatsuba226_255

    addi  a0, sp, 160   # a0
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 160   # a0
    call karatsuba226_255

    addi  a0, sp, 200   # a0[10]
    addi  a1, sp, 240   # a1
    addi  a2, sp, 200   # a0[10]
    call karatsuba226_255

    addi  a0, sp, 240   # a1
    addi  a1, sp, 200   # a0[10]
    addi  a2, sp, 160   # a0
    call add226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 200   # a0[10]
    addi  a2, sp, 160   # a0
    call sub226

    addi  a0, sp, 160   # a0
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 280   # a1[10]
    call karatsuba226_255

    addi  a0, sp, 40    # xzm1[10]
    addi  a1, sp, 160   # a0
    addi  a2, s0, 0     # work
    call karatsuba226_255

    addi  a0, sp, 0     # xzm1
    addi  a1, sp, 240   # a1
    addi  a2, sp, 240   # a1
    call karatsuba226_255

    addi  a0, sp, 240   # a1
    addi  a1, sp, 320   # b0
    addi  a2, sp, 360   # b0[10]
    call sub226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 240   # a1
    call mul121665asm

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 320   # b0
    call add226

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 320   # b0
    addi  a2, sp, 360   # b0[10]
    call karatsuba226_255

    addi  a0, sp, 120   # xzm[10]
    addi  a1, sp, 240   # a1
    addi  a2, sp, 280   # a1[10]
    call karatsuba226_255

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 80    # xzm
    addi  a3, sp, 0     # xzm1
    addi  a4, s4, 0     # b
    call crypto_select_asm

    addi  s3, s3, -1    # update j loop variable
    bgez  s3, firstloop

    addi  s2, s1, 32    # outerloop variable

outerloop:
    lwu    s5, 0(s2)     # e[i]
    li    s3, 25        # innerloop variable

innerloop:
    srl   s4, s5, s3    # e[9] >> loop variable
    andi  s4, s4, 1     # bit 234+j

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 80    # xzm
    addi  a3, sp, 0     # xzm1
    mv    a4, s4        # b
    call  crypto_select_asm

    addi  a0, sp, 160   # a0
    addi  a1, sp, 80    # xzm
    addi  a2, sp, 120   # xzm[10]
    call  add226

    addi  a0, sp, 200   # a0[10]
    addi  a1, sp, 80    # xzm
    addi  a2, sp, 120   # xzm[10]
    call sub226

    addi  a0, sp, 240   # a1
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 40    # xzm1[10]
    call add226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 40    # xzm1[10]
    call sub226

    addi  a0, sp, 320   # b0
    addi  a1, sp, 160   # a0
    addi  a2, sp, 160   # a0
    call karatsuba226_255

    addi  a0, sp, 360   # b0[10]
    addi  a1, sp, 200   # a0[10]
    addi  a2, sp, 200   # a0[10]
    call karatsuba226_255

    addi  a0, sp, 160   # a0
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 160   # a0
    call karatsuba226_255

    addi  a0, sp, 200   # a0[10]
    addi  a1, sp, 240   # a1
    addi  a2, sp, 200   # a0[10]
    call karatsuba226_255

    addi  a0, sp, 240   # a1
    addi  a1, sp, 160   # a0
    addi  a2, sp, 200   # a0[10]
    call add226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 200   # a0[10]
    addi  a2, sp, 160   # a0
    call sub226

    addi  a0, sp, 160   # a0
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 280   # a1[10]
    call karatsuba226_255

    addi  a0, sp, 40    # xzm1[10]
    addi  a1, sp, 160   # a0
    addi  a2, s0, 0     # work
    call karatsuba226_255

    addi  a0, sp, 0     # xzm1
    addi  a1, sp, 240   # a1
    addi  a2, sp, 240   # a1
    call karatsuba226_255

    addi  a0, sp, 240   # a1
    addi  a1, sp, 320   # b0
    addi  a2, sp, 360   # b0[10]
    call sub226

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 240   # a1
    call mul121665asm

    addi  a0, sp, 280   # a1[10]
    addi  a1, sp, 280   # a1[10]
    addi  a2, sp, 320   # b0
    call add226

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 320   # b0
    addi  a2, sp, 360   # b0[10]
    call karatsuba226_255

    addi  a0, sp, 120   # xzm[10]
    addi  a1, sp, 240   # a1
    addi  a2, sp, 280   # a1[10]
    call karatsuba226_255

    addi  a0, sp, 80    # xzm
    addi  a1, sp, 0     # xzm1
    addi  a2, sp, 80    # xzm
    addi  a3, sp, 0     # xzm1
    addi  a4, s4, 0     # b
    call crypto_select_asm

    addi  s3, s3, -1    # update j loop variable
    bgez  s3, innerloop

    addi  s2, s2, -4    # update i loop variable
    ble   s1, s2, outerloop

    lwu    t0, 80(sp)    # xzm[0]
    lwu    t1, 84(sp)    # xzm[1]
    lwu    t2, 88(sp)    # xzm[2]
    lwu    t3, 92(sp)    # xzm[3]
    lwu    t4, 96(sp)    # xzm[4]
    lwu    t5, 100(sp)   # xzm[5]
    lwu    t6, 104(sp)   # xzm[6]
    lwu    a0, 108(sp)   # xzm[7]
    lwu    a1, 112(sp)   # xzm[8]
    lwu    a2, 116(sp)   # xzm[9]
    sw    t0, 0(s0)     # work[0]
    sw    t1, 4(s0)     # work[1]
    sw    t2, 8(s0)     # work[2]
    sw    t3, 12(s0)    # work[3]
    sw    t4, 16(s0)    # work[4]
    sw    t5, 20(s0)    # work[5]
    sw    t6, 24(s0)    # work[6]
    sw    a0, 28(s0)    # work[7]
    sw    a1, 32(s0)    # work[8]
    sw    a2, 36(s0)    # work[9]

    lwu    t0, 120(sp)   # xzm[10]
    lwu    t1, 124(sp)   # xzm[11]
    lwu    t2, 128(sp)   # xzm[12]
    lwu    t3, 132(sp)   # xzm[13]
    lwu    t4, 136(sp)   # xzm[14]
    lwu    t5, 140(sp)   # xzm[15]
    lwu    t6, 144(sp)   # xzm[16]
    lwu    a0, 148(sp)   # xzm[17]
    lwu    a1, 152(sp)   # xzm[18]
    lwu    a2, 156(sp)   # xzm[19]
    sw    t0, 40(s0)    # work[10]
    sw    t1, 44(s0)    # work[11]
    sw    t2, 48(s0)    # work[12]
    sw    t3, 52(s0)    # work[13]
    sw    t4, 56(s0)    # work[14]
    sw    t5, 60(s0)    # work[15]
    sw    t6, 64(s0)    # work[16]
    sw    a0, 68(s0)    # work[17]
    sw    a1, 72(s0)    # work[18]
    sw    a2, 76(s0)    # work[19]

    lwu    s0, 428(sp)  # restore variables
    lwu    s1, 424(sp)  # 
    lwu    s2, 420(sp)  # 
    lwu    s3, 416(sp)  # 
    lwu    s4, 412(sp)  # 
    lwu    s5, 408(sp)  # 
    lwu    ra, 404(sp)  # 
    addi  sp, sp, 432  # 
    ret

