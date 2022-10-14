#ifndef KERNEL_API_H
#define KERNEL_API_H

#define KERNEL_ECALL_CODE_MIN (0x0)
#define KERNEL_ECALL_CODE_MAX (0x8)

#include <stdint.h>

#include "console.h"

void delegate_ecall_to_kernel(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc);

void illegal_instruction_trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);

void kernel_init(uintptr_t ftd_addr);

#endif // KERNEL_API_H
