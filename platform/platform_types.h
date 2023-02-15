#ifndef SM_PLATFORM_TYPES_H
#define SM_PLATFORM_TYPES_H

#include <stdint.h>
#include <platform_lock.h>

typedef uint64_t platform_core_state_t[32];

typedef struct {
  uintptr_t ev_base;
  uintptr_t ev_mask;

  uintptr_t meparbase;
  uintptr_t meparmask;
  
  uintptr_t eptbr;
} enclave_platform_csr_t;

typedef struct {
  uintptr_t ev_base;
  uintptr_t ev_mask;

  uintptr_t memrbm;

  uintptr_t meparbase;
  uintptr_t meparmask;
  
  uintptr_t eptbr;

  uintptr_t mie;
  uintptr_t mideleg;
  uintptr_t medeleg;
} thread_platform_csr_t;

typedef struct {
  volatile uint32_t* ipi;
  volatile int mipi_pending;

  volatile uint64_t* timecmp;

  volatile uint32_t* plic_m_thresh;
  volatile uint32_t* plic_m_ie;
  volatile uint32_t* plic_s_thresh;
  volatile uint32_t* plic_s_ie;
} hls_t;

#endif // SM_PLATFORM_TYPES_H
