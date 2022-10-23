.text

.globl crypto_verify_16
.type crypto_verify_16, @function
crypto_verify_16:

    lbu   t1, 0(a0)     # Load 3 from each value
    lbu   t2, 0(a1)     #
    lbu   t3, 1(a0)     #
    lbu   t4, 1(a1)     #
    lbu   t5, 2(a0)     #
    lbu   t6, 2(a1)     #

    xor   a2, t1, t2    # compare values
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #

    lbu   t1, 3(a0)     # Load 3 from each value
    lbu   t2, 3(a1)     #
    lbu   t3, 4(a0)     #
    lbu   t4, 4(a1)     #
    lbu   t5, 5(a0)     #
    lbu   t6, 5(a1)     #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 6(a0)     # Load 3 from each value
    lbu   t2, 6(a1)     #
    lbu   t3, 7(a0)     #
    lbu   t4, 7(a1)     #
    lbu   t5, 8(a0)     #
    lbu   t6, 8(a1)     #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 9(a0)     # Load 3 from each value
    lbu   t2, 9(a1)     #
    lbu   t3, 10(a0)    #
    lbu   t4, 10(a1)    #
    lbu   t5, 11(a0)    #
    lbu   t6, 11(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 12(a0)    # Load 3 from each value
    lbu   t2, 12(a1)    #
    lbu   t3, 13(a0)    #
    lbu   t4, 13(a1)    #
    lbu   t5, 14(a0)    #
    lbu   t6, 14(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    lbu   t1, 15(a0)    #
    lbu   t2, 15(a1)    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    
    xor   a3, t1, t2    #
    or    a2, a2, a3    #

    snez  a0, a2        # set 1 if not equal
    neg   a0, a0        # return -1 if not equal
    ret
