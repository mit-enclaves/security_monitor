.text

.globl crypto_verify_32
.type crypto_verify_32, @function
crypto_verify_32:

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
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 15(a0)    # Load 3 from each value
    lbu   t2, 15(a1)    #
    lbu   t3, 16(a0)    #
    lbu   t4, 16(a1)    #
    lbu   t5, 17(a0)    #
    lbu   t6, 17(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 18(a0)    # load 3 from each value
    lbu   t2, 18(a1)    #
    lbu   t3, 19(a0)    #
    lbu   t4, 19(a1)    #
    lbu   t5, 20(a0)    #
    lbu   t6, 20(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 21(a0)    # load 3 from each value
    lbu   t2, 21(a1)    #
    lbu   t3, 22(a0)    #
    lbu   t4, 22(a1)    #
    lbu   t5, 23(a0)    #
    lbu   t6, 23(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 24(a0)    # load 3 from each value
    lbu   t2, 24(a1)    #
    lbu   t3, 25(a0)    #
    lbu   t4, 25(a1)    #
    lbu   t5, 26(a0)    #
    lbu   t6, 26(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 27(a0)    # load 3 from each value
    lbu   t2, 27(a1)    #
    lbu   t3, 28(a0)    #
    lbu   t4, 28(a1)    #
    lbu   t5, 29(a0)    #
    lbu   t6, 29(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #
    xor   a3, t5, t6    #
    or    a2, a2, a3    #    

    lbu   t1, 30(a0)    # load 2 from each value
    lbu   t2, 30(a1)    #
    lbu   t3, 31(a0)    #
    lbu   t4, 31(a1)    #

    xor   a3, t1, t2    # compare values
    or    a2, a2, a3    #
    xor   a3, t3, t4    #
    or    a2, a2, a3    #

    snez  a0, a2        # set 1 if not equal to zero
    neg   a0, a0        # return -1 if not equal
    ret

