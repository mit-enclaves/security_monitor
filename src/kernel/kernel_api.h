#ifndef KERNEL_API_H
#define KERNEL_API_H

#define KERNEL_ECALL_CODE_MIN (0x0)
#define KERNEL_ECALL_CODE_MAX (0x9)

#include <stdint.h>
#include <platform_types.h>
#include "console.h"

void delegate_ecall_to_kernel(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc);

void illegal_instruction_trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);

void kernel_init(uintptr_t ftd_addr);
void kernel_init_other_core(uintptr_t core_id);

hls_t* hls_init(uintptr_t hart_id);
void send_ipi(uintptr_t recipient, int event);
void send_ipi_many(uintptr_t* pmask, int event);

void bad_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval);

#endif // KERNEL_API_H
