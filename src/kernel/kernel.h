#ifndef KERNEL_HELPERS_H
#define KERNEL_HELPERS_H

#include "kernel_api.h"
#include "encoding.h"
#include "csr/csr.h"
#include <stdint.h>
#include <stdbool.h>

typedef uintptr_t insn_t;

static inline uintptr_t get_insn(uintptr_t mepc, uintptr_t* mstatus)
{
  register uintptr_t __mstatus_adjust asm ("a1") = MSTATUS_MPRV | MSTATUS_MXR;
  register uintptr_t __mepc asm ("a2") = mepc;
  register uintptr_t __mstatus asm ("a3");
  uintptr_t val;

  asm ("csrrs %[mstatus], mstatus, %[mprv]\n"
       "lwu %[insn], (%[addr])\n"
       "csrw mstatus, %[mstatus]"
       : [mstatus] "+&r" (__mstatus), [insn] "=&r" (val)
       : [mprv] "r" (__mstatus_adjust), [addr] "r" (__mepc));
  *mstatus = __mstatus;
  return val;
}

#define EXTRACT_FIELD(val, which) (((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval) (((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#define SH_RD 7
#define SH_RS1 15
#define SH_RS2 20
#define SH_RS2C 2

#define LOG_REGBYTES 3

#define GET_RM(insn) (((insn) >> 12) & 7)

#define SHIFT_RIGHT(x, y) ((y) < 0 ? ((x) << -(y)) : ((x) >> (y)))
#define GET_REG(insn, pos, regs) ({ \
  int mask = (1 << (5+LOG_REGBYTES)) - (1 << LOG_REGBYTES); \
  (uintptr_t*)((uintptr_t)regs + (SHIFT_RIGHT(insn, (pos) - LOG_REGBYTES) & (mask))); \
})
#define GET_RS1(insn, regs) (*GET_REG(insn, SH_RS1, regs))
#define GET_RS2(insn, regs) (*GET_REG(insn, SH_RS2, regs))
#define GET_RS1S(insn, regs) (*GET_REG(RVC_RS1S(insn), 0, regs))
#define GET_RS2S(insn, regs) (*GET_REG(RVC_RS2S(insn), 0, regs))
#define GET_RS2C(insn, regs) (*GET_REG(insn, SH_RS2C, regs))
#define GET_SP(regs) (*GET_REG(2, 0, regs))
#define SET_RD(insn, regs, val) (*GET_REG(insn, SH_RD, regs) = (val))
#define IMM_I(insn) ((int32_t)(insn) >> 20)
#define IMM_S(insn) (((int32_t)(insn) >> 25 << 5) | (int32_t)(((insn) >> 7) & 0x1f))
#define MASK_FUNCT3 0x7000

#endif // KERNEL_HELPERS_H
