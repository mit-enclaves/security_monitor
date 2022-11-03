.globl mul121665asm
.type mul121665asm, @function
mul121665asm:

   addi    sp, sp, -32  # store variables
   sw      s0, 28(sp)   #
   sw      s1, 24(sp)   #
   sw      s2, 20(sp)   #
   sw      s3, 16(sp)   #
   sw      s10, 12(sp)  #

   lwu      t0, 0(a1)    # load A0
   mv      s0, a0       # store out address
   mv      s1, a1       # store A address

   li      t5, 0x3ffffff# 2^26-1
   li      t6, 121665   # load multiplicand

   lwu      t1, 4(s1)    # load A1
   mul     a0, t0, t6   # A0*121666_l
   mulh    a1, t0, t6   # A0*121666_h

   lwu      t3, 8(s1)    # Load A2
   mul     t2, t1, t6   # A1*121666_l
   mulh    a2, t1, t6   # A1*121666_h

   mul     t4, t3, t6   # A2*121666_l
   mulh    a3, t3, t6   # A2*121666_h

   srli    s10, a0, 26  # base change A0
   and     a0, a0, t5   #  r[0]
   slli    a1, a1, 6    #
   or      a1, a1, s10  #

   srli    s10, t2, 26  # base change A1
   and     t2, t2, t5   #
   slli    a2, a2, 6    #
   or      a2, a2, s10  #
   add     a1, a1, t2   # r[1]

   lwu      t0, 12(s1)   # load A3

   srli    s10, t4, 26  # base change A2
   and     t4, t4, t5   #

   mul     t2, t0, t6   # A3*121666_l
   mulh    a4, t0, t6   # A3*121666_h

   slli    a3, a3, 6    #
   or      a3, a3, s10  #
   add     a2, a2, t4   # r[2]

   lwu      t0, 16(s1)   # Load A4

   srli    s10, t2, 26  # base change A3
   and     t2, t2, t5   #

   mul     t1, t0, t6   # A4*121666_l
   mulh    a5, t0, t6   # A4*121666_h

   slli    a4, a4, 6    #
   or      a4, a4, s10  #
   add     a3, a3, t2   # r[3]

   lwu      t0, 20(s1)   # load A5

   srli    s10, t1, 26  # base change A4
   and     t1, t1, t5   #

   mul     t2, t0, t6   # A5*121666_l
   mulh    a6, t0, t6   # A5*121666_h

   slli    a5, a5, 6    #
   or      a5, a5, s10  #
   add     a4, a4, t1   # r[4]

   lwu      t0, 24(s1)   # load A6

   srli    s10, t2, 26  # base change A5
   and     t2, t2, t5   #

   mul     t1, t0, t6   # A6*121666_l
   mulh    a7, t0, t6   # A6*121666_h

   slli    a6, a6, 6    #
   or      a6, a6, s10  #
   add     a5, a5, t2   # r[5]

   lwu      t0, 28(s1)   # load A7

   srli    s10, t1, 26  # base change A6
   and     t1, t1, t5   #

   mul     t2, t0, t6   # A7*121666_l
   mulh    s2, t0, t6   # A7*121666_h

   slli    a7, a7, 6    #
   or      a7, a7, s10  #
   add     a6, a6, t1   # r[6]

   lwu      t0, 32(s1)   # load A8

   srli    s10, t2, 26  # base change A7
   and     t2, t2, t5   #

   mul     t1, t0, t6   # A8*121666_l
   mulh    s3, t0, t6   # A8*121666_h

   slli    s2, s2, 6    #
   or      s2, s2, s10  #
   add     a7, a7, t2   # r[7]

   lwu      t0, 36(s1)   # load A9

   li      t3, 0x1fffff #
   srli    s10, t1, 26  # base change A8
   and     t1, t1, t5   #

   mul     t2, t0, t6   # A9*121666_l
   mulh    t4, t0, t6   # A9*121666_h

   slli    s3, s3, 6    #
   or      s3, s3, s10  #
   add     s2, s2, t1   # r[8]

   srli    s10, t2, 26  # base change A9
   and     t2, t2, t5   #
   slli    t4, t4, 6    #
   or      t4, t4, s10  #
   add     s3, s3, t2   # r[9]
   srli    t2, s3, 26   #
   and     s3, s3, t5   #
   add     t4, t4, t2   #
   
   li      t2, 608      #
   mul     t4, t4, t2   # overflow * 608

   add     a0, a0, t4   # r[0] += overflow*19

#   srli    t0, a0, 26   # handle overflow
#   and     a0, a0, t5   #
#   add     a1, a1, t0   #
#   srli    t0, a1, 26   #
#   and     a1, a1, t5   #
#   add     a2, a2, t0   #
#   srli    t0, a2, 26   #
#   and     a2, a2, t5   #
#   add     a3, a3, t0   #
#   srli    t0, a3, 26   #
#   and     a3, a3, t5   #
#   add     a4, a4, t0   #
#   srli    t0, a4, 26   #
#   and     a4, a4, t5   #
#   add     a5, a5, t0   #
#   srli    t0, a5, 26   #
#   and     a5, a5, t5   #
#   add     a6, a6, t0   #
#   srli    t0, a6, 26   #
#   and     a6, a6, t5   #
#   add     a7, a7, t0   #
#   srli    t0, a7, 26   #
#   and     a7, a7, t5   #
#   add     s2, s2, t0   #
#   srli    t0, s2, 26   #
#   and     s2, s2, t5   #
#   add     s3, s3, t0   #
#   srli    t0, s3, 21   #
#
#   li t2, 19
#   mul     t0, t0, t2   # overflow *19
#   and     s3, s3, t3   #
#   add     a0, a0, t0   #
#
#   srli    t0, a0, 26   # handle overflow
#   and     a0, a0, t5   #
#   add     a1, a1, t0   #
#   srli    t0, a1, 26   #
#   and     a1, a1, t5   #
#   add     a2, a2, t0   #
#   srli    t0, a2, 26   #
#   and     a2, a2, t5   #
#   add     a3, a3, t0   #
#   srli    t0, a3, 26   #
#   and     a3, a3, t5   #
#   add     a4, a4, t0   #
#   srli    t0, a4, 26   #
#   and     a4, a4, t5   #
#   add     a5, a5, t0   #
#   srli    t0, a5, 26   #
#   and     a5, a5, t5   #
#   add     a6, a6, t0   #
#   srli    t0, a6, 26   #
#   and     a6, a6, t5   #
#   add     a7, a7, t0   #
#   srli    t0, a7, 26   #
#   and     a7, a7, t5   #
#   add     s2, s2, t0   #
#   srli    t0, s2, 26   #
#   and     s2, s2, t5   #
#   add     s3, s3, t0   #

   sw      a0, 0(s0)    # storing result
   sw      a1, 4(s0)    # 
   sw      a2, 8(s0)    #
   sw      a3, 12(s0)   #
   sw      a4, 16(s0)   #
   sw      a5, 20(s0)   #

   sw      a6, 24(s0)   #
   sw      a7, 28(s0)   #
   sw      s2, 32(s0)   #
   sw      s3, 36(s0)   #

   lwu      s0, 28(sp)   # restore variables
   lwu      s1, 24(sp)   #
   lwu      s2, 20(sp)   #
   lwu      s3, 16(sp)   #
   lwu      s10, 12(sp)  #
   addi    sp, sp, 32   #

   ret


