#ifndef CSR_H
#define CSR_H

#define STR_IMPL_(x) #x      //stringify
#define STR(x) STR_IMPL_(x)

#define CSR_RNG 0xCC0

// ### Enclave virtual base and mask
// (per-core) registers
// ( defines a virtual region for which enclave page tables are used in
//   place of OS-controlled page tables)
// (machine-mode non-standard read/write)
#define CSR_MEVBASE 0x7c0
#define CSR_MEVMASK 0x7c1

// ### Enclave page table base
// (per core) register
// ( pointer to a separate page table data structure used to translate enclave
//   virtual addresses)
// (machine-mode non-standard read/write)
#define CSR_MEATP 0x7c2

// ### DRAM bitmap
// (per core) registers (OS and Enclave)
// ( white-lists the DRAM regions the core is allowed to access via OS and
//   enclave virtual addresses)
// (machine-mode non-standard read/write)
#define CSR_MMRBM 0x7c3
#define CSR_MEMRBM 0x7c4

// ### Protected region base and mask
// (per core) registers (OS and Enclave)
// ( these are used to prevent address translation into a specific range of
//   physical addresses, for example to protect the security monitor from all software)
// (machine-mode non-standard read/write)
#define CSR_MPARBASE 0x7c5
#define CSR_MPARMASK 0x7c6
#define CSR_MEPARBASE 0x7c7
#define CSR_MEPARMASK 0x7c8

// Use double-macros so stringified argument can be expended by the pre-processor
#define read_csr(reg) _read_csr(reg)
#define _read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) _write_csr(reg, val)
#define _write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define swap_csr(reg, val) _swap_csr(reg, val)
#define _swap_csr(reg, val) ({ unsigned long __tmp; \
  asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); \
  __tmp;})

#define set_csr(reg, bit) _set_csr(reg, bit)
#define _set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) _clear_csr(reg, bit)
#define _clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#endif // CSR_H
