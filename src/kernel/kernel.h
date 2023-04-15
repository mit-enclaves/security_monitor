#ifndef KERNEL_HELPERS_H
#define KERNEL_HELPERS_H

#include "kernel_api.h"
#include "encoding.h"
#include "fdt.h"
#include "mcall.h"
#include "unprivileged_memory.h"
#include <clib/clib.h>
#include <platform.h>
#include <limits.h>

extern long disabled_hart_mask;
extern uintptr_t mem_size;
extern volatile uint64_t* mtime;
extern volatile uint32_t* plic_priorities;
extern size_t plic_ndevs;
extern void* kernel_start;
extern void* kernel_end;

typedef uintptr_t insn_t;

#define MACHINE_STACK_TOP() ({ \
  register uintptr_t sp asm ("sp"); \
  (char*)((sp + STACK_SIZE) & -STACK_SIZE); })

// hart-local storage, at top of stack
#define HLS() ((hls_t*)(MACHINE_STACK_TOP() - HLS_SIZE))
#define OTHER_HLS(id) ((hls_t*)((char*)HLS() - (STACK_SIZE * ((id) - read_csr(mhartid)))))

#define EXTRACT_FIELD(val, which) (((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval) (((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#define SH_RD 7
#define SH_RS1 15
#define SH_RS2 20
#define SH_RS2C 2

#define LOG_REGBYTES 3

#define GET_RM(insn) (((insn) >> 12) & 7)

#define GET_FUNCT3(insn) GET_RM(insn)
#define GET_FUNCT7(insn) (((insn) >> 25) & 0x7f)

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

#endif // KERNEL_HELPERS_H
