// See LICENSE for license details.

#ifndef _RISCV_SBI_H
#define _RISCV_SBI_H

#include <csr/csr.h>

typedef struct {
  volatile uint32_t* ipi;
  volatile int mipi_pending;

  volatile uint64_t* timecmp;

  volatile uint32_t* plic_m_thresh;
  volatile uintptr_t* plic_m_ie;
  volatile uint32_t* plic_s_thresh;
  volatile uintptr_t* plic_s_ie;
} hls_t;

#define MACHINE_STACK_TOP() ({ \
  register uintptr_t sp asm ("sp"); \
  (void*)((sp + PAGE_SIZE) & -PAGE_SIZE); })

// hart-local storage, at top of stack
#define HLS() ((hls_t*)(MACHINE_STACK_TOP() - HLS_SIZE))
#define OTHER_HLS(id) ((hls_t*)((void*)HLS() + PAGE_SIZE * ((id) - read_csr(mhartid))))

hls_t* hls_init(uintptr_t hart_id);

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2
#define SBI_CLEAR_IPI 3
#define SBI_SEND_IPI 4
#define SBI_REMOTE_FENCE_I 5
#define SBI_REMOTE_SFENCE_VMA 6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN 8

#endif
