#ifndef CONSTANTS_H
#define CONSTANTS_H

#define XLENINT uint64_t
#define SIZE_DRAM 0x80000000
#define SIZE_PAGE 0x1000
#define SHIFT_PAGE 12
#define NUM_CORES 2
#define NUM_REGIONS 64
#define MAILBOX_SIZE 128
#define PN_OFFSET 9
#define PPN2_OFFSET 26
#define PAGE_ENTRY_ACL_OFFSET 10

// CSR SPECIALS
#define REGBYTES 8

#define MIE_MEIE 0x800
#define MIP_MSIP 0x8
#define MIP_MTIP 0x80 
#define MIP_STIP 0x20
#define MIP_SSIP 0x2 

#define INTEGER_CONTEXT_SIZE 256 // TODO: is sp byte aligned?
#define TRAP_FROM_MACHINE_MODE_VECTOR 0 // TODO: deal with that

// HACKS

#define MENTRY_IPI_OFFSET 0
#define MENTRY_IPI_PENDING_OFFSET 0
#define IPI_SOFT 0
#define IPI_FENCE_I 0
#define IPI_SFENCE_VMA 0

#endif // CONSTANTS_H
