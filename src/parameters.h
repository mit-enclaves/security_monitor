#ifndef SM_PARAMETERS_H
#define SM_PARAMETERS_H

// SM Parameters:
// --------------

#define STACK_SIZE    (0x1000)
#define MAILBOX_SIZE  (0x1000)
#define NUM_UNTRUSTED_MAILBOXES (16)

/* Note: make sure the parameterization below is consistent with the target hardware platform! */

// Multicore configuration
#define NUM_CORES     (1)

// Region configuration
#define REGION_SHIFT  (25)
#define RAM_BASE      (0x80000000)
#define RAM_SIZE      (0x80000000)

// Paged virtual memory configuration
#define PAGE_SHIFT    (12)
#define PTE_SIZE    (8)
#define PN_OFFSET   (9)
#define PPN2_OFFSET (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

#endif // SM_PARAMETERS_H
