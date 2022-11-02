.text

.globl securemul226
.type securemul226, @function
securemul226:
    li    a4, 0x3ffffff # 2^26-1
    andi  t1, a1, 1     # get the last bit
    neg   t1, t1        # create mask
    and   a2, a0, t1    # r_l
    srli  a1, a1, 1     # update multiplier
    andi  t1, a1, 1     # get the last bit 
    neg   t1, t1        # create mask
    srli  t2, a0, 25    # shift multiplicand for the high part
    and   a3, t2, t1    # r_h, apply the mask
    slli  t2, a0, 1     # shift multiplicand for the low part
    and   t2, t2, a4    # only get the low 26 bits
    and   t2, t2, t1    # apply the mask
    add   a2, a2, t2    # add to r_l

    srli  a1, a1, 1     # update multiplier
    andi  t1, a1, 1     # get the last bit
    neg   t1, t1        # create mask
    srli  t2, a0, 24    # shift multiplicand for the high part
    and   t2, t2, t1    # apply the mask
    add   a3, a3, t2    # add to r_h
    slli  t2, a0, 2     # shift multiplicand for the low part
    and   t2, t2, a4    # only get the low 26 bits
    and   t2, t2, t1    # apply the mask
    add   a2, a2, t2    # add to r_l

    srli  a1, a1, 1     # update multiplier
    andi  t1, a1, 1     # get the last bit
    neg   t1, t1        # create mask
    srli  t2, a0, 23    # shift multiplicand for the high part
    and   t2, t2, t1    # apply the mask
    add   a3, a3, t2    # add to r_h
    slli  t2, a0, 3     # shift multiplicand for the low part
    and   t2, t2, a4    # only get the low 26 bits
    and   t2, t2, t1    # apply the mask
    add   a2, a2, t2    # add to r_l

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 22    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 4     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 21    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 5     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 20    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 6     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 19    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 7     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 18    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 8     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 17    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 9     # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 16    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 10    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 15    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 11    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 14    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 12    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 13    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 13    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 12    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 14    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 11    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 15    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 10    # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 16    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 9     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 17    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 8     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 18    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 7     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 19    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 6     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 20    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 5     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 21    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 4     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 22    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 3     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 23    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 2     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 24    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  a1, a1, 1     # 
    andi  t1, a1, 1     # 
    neg   t1, t1        # 
    srli  t2, a0, 1     # 
    and   t2, t2, t1    # 
    add   a3, a3, t2    # 
    slli  t2, a0, 25    # 
    and   t2, t2, a4    # 
    and   t2, t2, t1    # 
    add   a2, a2, t2    # 

    srli  t0, a2, 26    # handle overflow
    add   a1, a3, t0    # r_h
    and   a0, a2, a4    # r_l
    ret

