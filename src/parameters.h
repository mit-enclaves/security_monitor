#ifndef SM_PARAMETERS_H
#define SM_PARAMETERS_H

// SM Parameters:
// --------------

#define STACK_SIZE    (0x1000)
#define MAILBOX_SIZE  (0x100)
#define NUM_UNTRUSTED_MAILBOXES (8)
#define CLEAN_REGIONS_ON_FREE   (true)
/* Note: make sure the parameterization below is consistent with the target hardware platform! */

// Placemetn of SM in memory
#define SM_STATE_ADDR   0x80001000
#define SM_STATE_LEN        0x3000

#define SM_ADDR         0x80004000
#define HANDLER_LEN         0x2000
#define SM_LEN              0x5000

#define UNTRUSTED_ENTRY 0x80010000

// Machine configuration
#define RAM_BASE        0x80000000
#define RAM_SIZE        0x80000000

#define REGION_SHIFT  (25)

#define NUM_CORES     (1)

// Paged virtual memory configuration
#define PAGE_SHIFT    (12)
#define PTE_SIZE      (8)
#define PN_OFFSET     (9)
#define PPN2_OFFSET   (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

#endif // SM_PARAMETERS_H
