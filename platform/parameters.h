#ifndef SM_PARAMETERS_H
#define SM_PARAMETERS_H

// SM Parameters:
// --------------

#define BOOT_ADDR (0x1000)
#define BOOT_MAXLEN (0x10)

#define MAILBOX_SIZE  (0x100)
#define NUM_UNTRUSTED_MAILBOXES (8)
#define CLEAN_REGIONS_ON_FREE   (true)

// Stack Parameter
#define STACK_SIZE          (0x1000)
#define INTEGER_CONTEXT_SIZE (0x100)
#define HLS_SIZE                 64
#define MENTRY_FRAME_SIZE (INTEGER_CONTEXT_SIZE + HLS_SIZE)

/* Note: make sure the parameterization below is consistent with the target hardware platform! */

// Placement for HTIF
#define HTIF_BASE 0x10001000
#define HTIF_LEN        0x10

// Placemetn of SM in memory
#define SM_STATE_ADDR   0x80000000
#define SM_STATE_LEN        0x3000

#define SM_ADDR         0x80003000
#define HANDLER_LEN         0x3000
#define SM_LEN             0x20000

#define UNTRUSTED_ENTRY  0x82000000
#define PAYLOAD_MAXLEN    0xC000000

// Machine configuration
#define RAM_BASE        0x80000000
#define RAM_SIZE        0x80000000

#define REGION_SHIFT  (25)

#define NUM_CORES     (1)

// SATP configuration
#define SATP_MODE_SV39 (8ul)
#define SATP_MODE (60)

// MSTATUS configuration
#define MSTATUS_TVM_MASK   (0x00100000)
#define MSTATUS_MPP_MASK   (0x00001800)
#define MSTATUS_MPP_OFFSET (11)
#define MSTATUS_MPIE_MASK  (0x00000080)
#define MSTATUS_SIE_MASK   (0x00000002)
#define MSTATUS_UIE_MASK   (0x00000001)

// Paged virtual memory configuration

#define IDPT_BASE (0x8D000000)
#define IDPT_SIZE (0x4000)

#define LEAF_ACL (0b11111111) // D A G U X W R V
#define NODE_ACL (0b00000001) // Node

#define PAGE_SHIFT    (12)
#define PTE_SIZE      (8)
#define PN_OFFSET     (9)
#define PPN2_OFFSET   (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

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
