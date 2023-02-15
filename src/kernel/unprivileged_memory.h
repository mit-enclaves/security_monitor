// See LICENSE for license details.

#ifndef _RISCV_MISALIGNED_H
#define _RISCV_MISALIGNED_H

#include "encoding.h"
#include <stdint.h>

#define DECLARE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)              \
  static inline type load_##type(const type* addr, uintptr_t mepc)  \
  {                                                                 \
    register uintptr_t __mstatus_adjust asm ("a1") = MSTATUS_MPRV;  \
    register uintptr_t __mepc asm ("a2") = mepc;                    \
    register uintptr_t __mstatus asm ("a3");                        \
    type val;                                                       \
    asm ("csrrs %0, mstatus, %3\n"                                  \
         #insn " %1, %2\n"                                          \
         "csrw mstatus, %0"                                         \
         : "+&r" (__mstatus), "=&r" (val)                           \
         : "m" (*addr), "r" (__mstatus_adjust), "r" (__mepc));      \
    return val;                                                     \
  }

#define DECLARE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                 \
  static inline void store_##type(type* addr, type val, uintptr_t mepc) \
  {                                                                     \
    register uintptr_t __mstatus_adjust asm ("a1") = MSTATUS_MPRV;      \
    register uintptr_t __mepc asm ("a2") = mepc;                        \
    register uintptr_t __mstatus asm ("a3");                            \
    asm volatile ("csrrs %0, mstatus, %3\n"                             \
                  #insn " %1, %2\n"                                     \
                  "csrw mstatus, %0"                                    \
                  : "+&r" (__mstatus)                                   \
                  : "r" (val), "m" (*addr), "r" (__mstatus_adjust),     \
                    "r" (__mepc));                                      \
  }

DECLARE_UNPRIVILEGED_LOAD_FUNCTION(uint8_t, lbu)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(uint16_t, lhu)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(int8_t, lb)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(int16_t, lh)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(int32_t, lw)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(uint8_t, sb)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(int8_t, sb)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(uint16_t, sh)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(uint32_t, sw)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(uint32_t, lwu)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(uint64_t, ld)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(uint64_t, sd)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(uintptr_t, ld)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(uintptr_t, sd)


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

#endif
