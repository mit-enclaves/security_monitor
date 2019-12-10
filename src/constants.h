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

// SM API Calls
// ------------
#define SM_ENCLAVE_CREATE                   (1000)
#define SM_ENCLAVE_DELETE                   (1001)
#define SM_ENCLAVE_ENTER                    (1002)
#define SM_ENCLAVE_EXIT                     (1003)
#define SM_ENCLAVE_INIT                     (1004)
#define SM_ENCLAVE_LOAD_HANDLER             (1005)
#define SM_ENCLAVE_LOAD_PAGE_TABLE_ENTRY    (1006)
#define SM_ENCLAVE_LOAD_PAGE_TABLE          (1007)
#define SM_ENCLAVE_LOAD_PAGE                (1008)

#define SM_GET_ATTESTATION_KEY              (1010)
#define SM_GET_PUBLIC_FIELD                 (1011)

#define SM_MAIL_ACCEPT                      (1020)
#define SM_MAIL_RECEIVE                     (1021)
#define SM_MAIL_SEND                        (1022)

#define SM_REGION_ASSIGN                    (1030)
#define SM_REGION_BLOCK                     (1031)
#define SM_REGION_FREE                      (1032)
#define SM_REGION_METADATA_CREATE           (1033)
#define SM_REGION_OWNER                     (1034)
#define SM_REGION_STATE                     (1035)

#define SM_THREAD_ALLOCATE                  (1040)
#define SM_THREAD_ASSIGN                    (1041)
#define SM_THREAD_DELETE                    (1042)
#define SM_THREAD_LOAD                      (1043)

#endif // CONSTANTS_H
