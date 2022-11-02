// See LICENSE for license details.

#ifndef _RISCV_SBI_H
#define _RISCV_SBI_H

#include <csr/csr.h>
#include <platform.h>


hls_t* hls_init(uintptr_t hart_id);
void send_ipi(uintptr_t recipient, int event);
void send_ipi_many(uintptr_t* pmask, int event);

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2
#define SBI_CLEAR_IPI 3
#define SBI_SEND_IPI 4
#define SBI_REMOTE_FENCE_I 5
#define SBI_REMOTE_SFENCE_VMA 6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN 8
#define SBI_EXIT 9

#endif
