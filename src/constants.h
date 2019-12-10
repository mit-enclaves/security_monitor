#ifndef CONSTANTS_H
#define CONSTANTS_H

// SM Parameters:
// --------------


#define STACK_SIZE    (0x1000)
#define MAILBOX_SIZE  (0x1000)
#define NUM_MAILBOXES (0x1000)

/* Note: make sure the parameterization below is consistent with the target hardware platform! */

#define NUM_CORES     (1)
#define PAGE_SHIFT    (12)
#define REGION_SHIFT  (25)
#define RAM_BASE      (0x80000000)
#define RAM_SIZE      (0x80000000)


// Derived values:
// ---------------

#define PAGE_SIZE (1<<PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE-1)

#define REGION_SIZE (1<<REGION_SHIFT)
#define REGION_MASK (REGION_SIZE-1)
#define NUM_REGIONS (RAM_SIZE / REGION_SIZE)

#define PTE_SIZE    (8)
#define PN_OFFSET   (9)
#define PPN2_OFFSET (26)
#define PAGE_ENTRY_ACL_OFFSET (10)

#endif // CONSTANTS_H
