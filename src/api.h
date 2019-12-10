#ifndef SECURITY_MONITOR_API_COMMON_H
#define SECURITY_MONITOR_API_COMMON_H

#include <stdint.h>
#include <stdbool.h>

// SM API Return values
// --------------------

typedef enum {
   // API call succeeded.
   monitor_ok = 0,

   // A parameter given to the API call was invalid.
   //
   // This most likely reflects a bug in the caller code.
   monitor_invalid_value = 1,

   // A resource referenced by the API call is in an unsuitable state.
   //
   // The API call will not succeed if the caller simply retries it. However,
   // the caller may be able to perform other API calls to get the resources in
   // a state that will allow this call to succeed.
   monitor_invalid_state = 2,

   // Failed to acquire a lock. Retrying with the same arguments might succeed.
   //
   // The monitor returns this instead of blocking a hardware thread when a
   // resource lock is acquired by another thread. This approach eliminates any
   // possibility of having the monitor deadlock. The caller is responsible for
   // retrying the API call.
   //
   // This is also sometime returned instead of monitor_invalid_value, in the
   // interest of reducing edge cases in monitor implementation.
   monitor_concurrent_call = 3,

   // The call was interrupted due to an asynchronous enclave exit (AEX).
   //
   // This is only returned by enter_enclave, and can be considered a more
   // specific case of monitor_concurrent_call. The caller should retry the
   // enclave_enter call, so the enclave thread can make progress.
   monitor_async_exit = 4,

   // The caller is not allowed to access a resource referenced by the API call.
   //
   // This is a more specific version of monitor_invalid_value. The monitor does
   // its best to identify these cases, but may fail.
   monitor_access_denied = 5,

   // The current monitor implementation does not support the request.
   //
   // The caller made a reasonable API request that exercises an unhandled edge
   // case in the monitor implementaiton. Some edge cases that would require
   // complex or difficult-to-test implementations are detected and handled by
   // returning monitor_unsupported.
   //
   // The documentation for API calls states the edge cases that result in a
   // monitor_unsupported response.
   monitor_unsupported = 6,
} api_result_t;

typedef uint64_t region_id_t;

typedef enum {
  PUBLIC_FIELD_PK_M = 0,
  PUBLIC_FIELD_PK_D = 1,
  PUBLIC_FIELD_PK_SM = 2,
  PUBLIC_FIELD_H_SM = 3,
  PUBLIC_FIELD_SIG_M = 4,
  PUBLIC_FIELD_SIG_D = 5,
  PUBLIC_FIELD_H_AE = 6,
} public_field_t;

typedef enum {
  REGION_STATE_INVALID = 0,
  REGION_STATE_FREE = 1,
  REGION_STATE_BLOCKED = 2,
  REGION_STATE_LOCKED = 3,
  REGION_STATE_OWNED = 4,
} dram_region_state_t;

typedef enum {
  REGION_TYPE_UNTRUSTED = 0,
  REGION_TYPE_ENCLAVE = 1,
  REGION_TYPE_METADATA = 2,
  REGION_TYPE_SM = 3,
} dram_region_type_t;

typedef enum { // NOTE must fit into 12 bits
  METADATA_INVALID = 0,
  METADATA_FREE = 1,
  METADATA_ENCLAVE = 2,
  METADATA_THREAD = 3,
} metadata_page_t;


// APIs: SM Enclave-related calls
// ------------------------------

// Creates an enclave's metadata structure.
//
// `enclave_id` must be the physical address of the first page in a sequence of
// free pages in the same DRAM metadata region stripe. It becomes the enclave's
// ID used for subsequent API calls. The required number of free metadata pages
// can be obtained by calling `enclave_metadata_pages`.
//
// `ev_base` and `ev_mask` indicate the range of enclave virtual addresses. The
// addresses this range get translated using the enclave page tables, and must
// point into enclave memory.
//
// `mailbox_count` is the number of mailboxes that the enclave will have. Valid
// mailbox IDs for this enclave will range from 0 to mailbox_count - 1.
//
// `debug` is set for debug enclaves. A security monitor that supports
// enclave debugging implements copy_debug_enclave_page, which can only be used
// on debug enclaves.
//
// All arguments become a part of the enclave's measurement.
api_result_t sm_enclave_create (enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug);

// Frees up all DRAM regions and the metadata associated with an enclave.
//
// This can only be called when there is no thread metadata associated with the
// enclave.
api_result_t sm_enclave_delete (enclave_id_t enclave_id);

// Starts executing enclave code on the current hardware thread.
//
// The application thread performing this system call will be suspended until
// the enclave code executes an enclave exit, or is interrupted by an
// asynchronous enclave exit.
//
// `enclave_id` must identify an enclave that has been initialized.
//
// `thread_id` must identify a hardware thread that was created but is not
// executing on any core.
api_result_t sm_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id);

// Ends the currently running enclave thread and returns control to the OS.
api_result_t sm_enclave_exit ();

// Marks the given enclave as initialized and ready to execute.
//
// `enclave_id` must identify an enclave that has not yet been initialized.
api_result_t sm_enclave_init (enclave_id_t enclave_id);

// Copies the SM trap handler code into a newly created enclave.
//
// `enclave_id` must identify an enclave that has not yet been initialized, and one that has not yet had any other load opeartion performed.
api_result_t sm_enclave_load_handler (enclave_id_t enclave_id);

// Allocates a page in the enclave's main DRAM region for page tables.
//
// `enclave_id` must be an enclave that has not yet been initialized.
//
// `phys_addr` must be higher than the last physical address passed to a
// load_enclave_ function, must be page-aligned, and must point into a DRAM
// region owned by the enclave.
//
// `virtual_addr` is the lowest virtual address mapped by the newly created
// page table.
//
// `level` indicates the page table level (e.g., in x86, 0 for PT, 1 for PD, 2
// for PDPT, 3 for PML).
//
// `virtual_addr`, `level` and `acl` become a part of the enclave's
// measurement.
api_result_t load_page_table(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uint64_t level, uintptr_t acl);

// Allocates and initializes a page in the enclave's main DRAM region.
//
// `enclave_id` must be an enclave that has not yet been initialized.
//
// `phys_addr` must be higher than the last physical address passed to a
// load_enclave_ function, must be page-aligned, and must point into a DRAM
// region owned by the enclave.
//
// `virtual_addr`, `acl`, and the contents of the page at `os_addr` become a
// part of the enclave's measurement.
api_result_t load_page(enclave_id_t enclave_id, uintptr_t phys_addr,
      uintptr_t virtual_addr, uintptr_t os_addr, uintptr_t acl);

// Returns the number of pages used by an enclave metadata structure.
uint64_t sm_enclave_metadata_pages(uint64_t mailbox_count);

// APIs: Getters for SM values
// ---------------------------

// Reads the monitor's private attestation key.
//
// This API call will only succeed if the calling enclave is the special
// enclave designated by the security monitor to be the signing enclave.
//
// `phys_addr` must point into a buffer large enough to store the attestation
// key. The entire buffer must be contained in a single DRAM region that
// belongs to the enclave.
api_result_t sm_get_attestation_key (uintptr_t phys_addr);

// Reads the monitor's requested public field.
//
// No special permissions are required
//
// `phys_addr` must point into a buffer large enough to store the requested
// field. The buffer must be contained in a single DRAM region that
// belongs to the enclave.
api_result_t sm_get_public_field (public_field_t field, uintptr_t phys_addr);


// APIs: SM Mail-related calls
// ---------------------------

// Prepares a mailbox to receive a message from another enclave.
//
// The mailbox will discard any message that it might contain.
api_result_t sm_mail_accept (mailbox_id_t mailbox_id, enclave_id_t expected_sender);

// Attempts to read a message received in a mailbox.
//
// If the read succeeds, the mailbox will transition into the free state.
//
// `phys_addr` must point into a buffer large enough to store a
// mailbox_identity_t structure. The entire buffer must be contained in a
// single DRAM region that belongs to the enclave.
api_result_t sm_mail_receive (mailbox_id_t mailbox_id, uintptr_t phys_addr);

// Sends a message to another enclave's mailbox.
//
// `enclave_id` and `mailbox_id` identify the destination mailbox.
//
// `phys_addr` must point into a buffer large enough to store a
// mailbox_identity_t structure. The entire buffer must be contained in a
// single DRAM region that belongs to the enclave.
//
// The structure contains the destination enclave's expected identity. The
// monitor will refuse to deliver the message
api_result_t sm_mail_send (enclave_id_t enclave_id, mailbox_id_t mailbox_id,
      uintptr_t phys_addr);


// APIs: SM Region-related calls
// -----------------------------

// Assigns a free DRAM region to an enclave or to the OS.
//
// `new_owner` is the enclave ID of the enclave that will own the DRAM region.
// 0 means that the DRAM region will be assigned to the OS.
//
api_result_t sm_region_assign (dram_region_id_t id, enclave_id_t new_owner);

// Blocks a DRAM region that was previously owned by the caller.
//
// After this call completes, the caller should not expect to be able to access
// the memory inside the blocked DRAM region. The memory might still be
// accessible via stale TLB entries, but these entries will be removed before
// the DRAM region is freed.
//
// Enclaves calling this API are responsible for wiping any confidential
// information from the relinquished DRAM region.
//
// Before issuing this call, the OS is responsible for wiping its own
// confidential information from the DRAM region.
api_result_t sm_region_block (dram_region_id_t id);

//namespace enclave {  // sanctum::api::enclave

// Returns monitor_ok if the given DRAM region is owned by the calling enclave.
//
// This is used by enclaves to confirm that they own a DRAM region when the OS
// tells them that they do. The enclave should that assume something went wrong
// if it sees any return value other than monitor_ok.
api_result_t sm_region_check_owned (dram_region_id_t id);

// Performs the TLB flushes needed to free a locked region.
//
// System software must invoke this call instead of flushing the TLB directly,
// as the monitor's state must be updated to reflect the fact that a TLB flush
// has occurred.
api_result_t sm_region_flush ();

// Frees a DRAM region that was previously locked.
api_result_t sm_region_free (dram_region_id_t id);

// Reserves a free DRAM region to hold enclave metadata.
//
// DRAM regions that hold enclave metadata can be freed directly by calling
// free_dram_region(). Calling block_dram_region() on them will fail.
api_result_t sm_region_metadata_create (dram_region_id_t dram_region);

// Returns the number of addressable metadata pages in a DRAM metadata region.
//
// This may be smaller than the number of total pages in a DRAM region, if the
// computer does not have continuous DRAM regions and the security monitor does
// not support using non-continuous regions.
uint64_t sm_region_metadata_pages();

// Returns the first usable metadata page in a DRAM metadata region.
//
// The beginning of each DRAM metadata region is reserved for the monitor's
// use. This returns the first page number that can be used to store
// enclave_info_t and thread_info_t structures.
uint64_t sm_region_metadata_start();

// Returns the owner of the DRAM region with the given index.
//
// Returns null_enclave_id if the given DRAM region index is invalid, locked by
// another operation, or if the region is not in the owned state.
enclave_id_t sm_region_owner (dram_region_id_t id);

// Returns the state of the DRAM region with the given index.
//
// Returns dram_region_invalid if the given DRAM region index is invalid.
// Returns dram_region_locked if the given DRAM region is currently locked by
// another API call.
dram_region_state_t sm_region_state (dram_region_id_t id);


// APIs: SM Thread-related calls
// -----------------------------

// Creates a hardware thread using metadata pages assigned by the OS.
//
// `thread_id` is a thread ID used by the OS in an `assign_thread` call that
// assigned metadata pages to this enclave. Enclaves can safely pass any value
// supplied by the OS as a parameter to this call, but must be prepared to
// handle an error code, which may occur if the OS supplies an incorrect value.
//
// `thread_info_addr` is the physical address of a thread_init_info_t structure
// that will be used to initialize the thread's metadata. The address must be
// page-aligned. The memory used for the thread_init_info_t structure can be
// reused for other purposes once the API call returns.
api_result_t sm_thread_accept (thread_id_t thread_id, uintptr_t thread_info_addr);

// Allocates a thread metadata structure to be used by an enclave.
//
// `enclave_id` must be an enclave that has been initialized and has not yet
// been killed.
//
// `thread_id` must be the physical address of the first page in a sequence of
// free pages in the same DRAM metadata region. It becomes the thread's ID used
// for subsequent API calls. The required number of free metadata pages can be
// obtained by calling `thread_metadata_pages`.
api_result_t sm_thread_assign (enclave_id_t enclave_id, thread_id_t thread_id);

// Deallocates a thread info slot.
//
// The thread must not be running on any core.
api_result_t sm_thread_delete (thread_id_t thread_id);

// Creates a hardware thread in an enclave.
//
// `enclave_id` must be an enclave that has not yet been initialized.
//
// `thread_id` must be the physical address of the first page in a sequence of
// free pages in the same DRAM metadata region stripe. It becomes the thread's
// ID used for subsequent API calls. The required number of free metadata pages
// can be obtained by calling `thread_metadata_pages`.
//
// `entry_pc`, `entry_stack`, `fault_pc` and `fault_stack` are virtual
// addresses in the enclave's address space. They are used to set the
// corresponding fields in thread_init_info_t.
//
// This must be called after the enclave's root page table is set by a call to
// load_page_table().
api_result_t load_thread (enclave_id_t enclave_id, thread_id_t thread_id,
    uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc,
    uintptr_t fault_stack);

// Returns the number of pages used by a thread metadata structure.
uint64_t sm_thread_metadata_pages();


#endif // SECURITY_MONITOR_API_COMMON_H
