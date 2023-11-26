#ifndef SM_PARAMETERS_H
#define SM_PARAMETERS_H

#include <csr/csr.h>

// SM Parameters:
// --------------

#define BOOT_ADDR (0x1000)
#define BOOT_MAXLEN (0x20)

#define MAILBOX_SIZE  (0x100)
#define NUM_UNTRUSTED_MAILBOXES (8)

// Stack Parameter
#define STACK_SIZE          (0x1000)
#define INTEGER_CONTEXT_SIZE (0x100)
#define HLS_SIZE                 64
#define MENTRY_FRAME_SIZE (INTEGER_CONTEXT_SIZE + HLS_SIZE)

/* Note: make sure the parameterization below is consistent with the target hardware platform! */

// Placement for HTIF
#define HTIF_BASE 0x10001000
#define HTIF_LEN        0x10

// Placement of SM in memory
#define SM_ADDR         0x80000000 // Should be region aligned
#define HANDLER_LEN         0x8000 // Should be a power of 2
#define SM_LEN             0x30000

#define SM_STATE_ADDR   0x80040000
#define SM_STATE_LEN        0x3000

#define SM_LAST_ADDRESS (SM_STATE_ADDR + SM_STATE_LEN)

#define UNTRUSTED_ENTRY  0x82000000
#define PAYLOAD_MAXLEN    0xC000000

//// Machine configuration

// Number of Cores
#define NUM_CORES (2)

// Memory
#define RAM_BASE        0x80000000
#define RAM_SIZE        0x80000000

// LLC partition controller
#define LLC_CTRL_ADDR        0x200000000
#define LLC_CTRL_ID_OFFSET   14
#define LLC_CTRL_BASE_OFFSET 4
#define LLC_CTRL_SIZE_OFFSET 0

// LLC
#define LLC_INDEX_OFFSET 6
#define LLC_NUM_WAYS 16

// Zero-device for LLC flush
#define ZERO_DEVICE_OFFSET 0x100000000

#define REGION_SHIFT  (25)

// MSPEC configuration
#define MSPEC_ALL    (0UL)
#define MSPEC_NONMEM (1UL)
#define MSPEC_NONE   (3UL)
#define MSPEC_NOTRAINPRED (4UL)
#define MSPEC_NOUSEPRED (8UL)
#define MSPEC_NOUSEL1 (16UL)

// SATP configuration
#define SATP_MODE_SV39 (8ul)
#define SATP_MODE (60)

// MSTATUS configuration
#define MSTATUS_TVM_MASK   (0x00100000)
#define MSTATUS_MPP_MASK   (0x00001800)
#define MSTATUS_MPP_OFFSET (11)
#define MSTATUS_MPIE_MASK  (0x00000080)
#define MSTATUS_MIE_MASK   (0x00000008)
#define MSTATUS_SIE_MASK   (0x00000002)
#define MSTATUS_UIE_MASK   (0x00000001)

// MIE configuration
#define MIE_MEIE_MASK (0x00000200)
#define MIE_SEIE_MASK (0x00000800)

// Paged virtual memory configuration

#define IDPT_BASE (0x8D000000)
#define IDPT_SIZE (0x4000)

#define LEAF_ACL (0b11101111) // D A G U X W R V
#define NODE_ACL (0b00000001) // Node

#define PAGE_SHIFT    (12)
#define PTE_SIZE      (8)
#define PN_OFFSET     (9)
#define PPN2_OFFSET   (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

#define MVBASE_DEFAULT (0xFFFFFFFFFFFFFFFF)
#define MVMASK_DEFAULT (0)

// Register file parameters
#define NUM_REGISTERS (32)
#define REGBYTES      (8)

// MIP CSR Fields
#define IRQ_S_SOFT   (1)
#define IRQ_H_SOFT   (2)
#define IRQ_M_SOFT   (3)
#define IRQ_S_TIMER  (5)
#define IRQ_H_TIMER  (6)
#define IRQ_M_TIMER  (7)
#define IRQ_S_EXT    (9)
#define IRQ_H_EXT    (10)
#define IRQ_M_EXT    (11)
#define IRQ_COP      (12)
#define IRQ_HOST     (13)

#endif // SM_PARAMETERS_H