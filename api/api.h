#ifndef SECURITY_MONITOR_API_COMMON_H
#define SECURITY_MONITOR_API_COMMON_H

#include "api_types.h"
#include "api_util.h"

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
// `num_mailboxes` is the number of mailboxes that the enclave will have. Valid
// mailbox IDs for this enclave will range from 0 to num_mailboxes - 1.
//
// `debug` is set for debug enclaves. A security monitor that supports
// enclave debugging implements copy_debug_enclave_page, which can only be used
// on debug enclaves.
//
// All arguments become a part of the enclave's measurement.
api_result_t sm_enclave_create (
  enclave_id_t enclave_id,
  uintptr_t ev_base,
  uintptr_t ev_mask,
  uint64_t num_mailboxes,
  bool debug);

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
api_result_t sm_enclave_enter (enclave_id_t enclave_id, thread_id_t thread_id, uintptr_t *regs); // TODO rename the handles etc...

// Ends the currently running enclave thread and returns control to the OS.
api_result_t sm_enclave_exit (void);

// Marks the given enclave as initialized and ready to execute.
//
// `enclave_id` must identify an enclave that has not yet been initialized.
api_result_t sm_enclave_init (enclave_id_t enclave_id);

// Copies the SM trap handler code into a newly created enclave.
//
// `enclave_id` must identify an enclave that has not yet been initialized, and one that has not yet had any other load opeartion performed.
api_result_t sm_enclave_load_handler (enclave_id_t enclave_id, uintptr_t phys_addr);

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
api_result_t sm_enclave_load_page_table (
  enclave_id_t enclave_id,
  phys_ptr_t phys_addr,
  uintptr_t virtual_addr,
  uint64_t level,
  uintptr_t acl);

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
api_result_t sm_enclave_load_page (
  enclave_id_t enclave_id,
  uintptr_t phys_addr,
  uintptr_t virtual_addr,
  uintptr_t os_addr,
  uintptr_t acl);

// Returns the number of pages used by an enclave metadata structure.
uint64_t sm_enclave_metadata_pages (uint64_t num_mailboxes);

// Request the SM for the enclave's local attestation.
//
// `enclave_id` must identify an enclave that has been initialized.
// `addr_*` are virtual addresses and will go through the hardware memory checks
api_result_t sm_enclave_get_attest (enclave_id_t enclave_id, void *addr_measurement, void *addr_pk, void *addr_attest);

// Request the SM for the enclave's keys.
//
// The enclave is identified as the caller.
// `addr_*` are virtual addresses and will go through the hardware memory checks
api_result_t sm_enclave_get_keys (void *addr_measurement, void *addr_pk, void *addr_sk, void *addr_attest);

// APIs: Getters for SM values
// ---------------------------

// Reads the monitor's requested public field.
//
// No special permissions are required
//
// `addr` is a virtual addresse and will go through the hardware memory checks
api_result_t sm_get_public_field (public_field_t field, void *addr);


// APIs: SM Mail-related calls
// ---------------------------

// Prepares a mailbox to receive a message from another enclave.
//
// The mailbox will discard any message that it might contain.
api_result_t sm_mail_accept (
  mailbox_id_t mailbox_id,
  enclave_id_t expected_sender);

// Attempts to read a message received in a mailbox.
//
// If the read succeeds, the mailbox will transition into the free state.
api_result_t sm_mail_receive (mailbox_id_t mailbox_id, void * out_message, void * out_sender_measurement);

// Sends a message to another enclave's mailbox.
//
// `enclave_id` and `mailbox_id` identify the destination mailbox.
//
// The structure contains the destination enclave's expected identity. The
// monitor will refuse to deliver the message
api_result_t sm_mail_send (
  enclave_id_t enclave_id,
  mailbox_id_t mailbox_id,
  void * addr);


// APIs: SM Region-related calls
// -----------------------------

// Assigns a free DRAM region to an enclave or to the OS.
//
// `new_owner` is the enclave ID of the enclave that will own the DRAM region.
// 0 means that the DRAM region will be assigned to the OS.
//
api_result_t sm_region_assign (region_id_t id, enclave_id_t new_owner);

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
api_result_t sm_region_block (region_id_t id);

//namespace enclave {  // sanctum::api::enclave

// Returns monitor_ok if the given DRAM region is owned by the calling enclave.
//
// This is used by enclaves to confirm that they own a DRAM region when the OS
// tells them that they do. The enclave should that assume something went wrong
// if it sees any return value other than monitor_ok.
api_result_t sm_region_check_owned (region_id_t id);

// Performs the TLB flushes needed to free a locked region.
//
// System software must invoke this call instead of flushing the TLB directly,
// as the monitor's state must be updated to reflect the fact that a TLB flush
// has occurred.
api_result_t sm_region_flush (void);

// Frees a DRAM region that was previously blocked.
api_result_t sm_region_free (region_id_t id);

// Reserves a free DRAM region to hold enclave metadata.
//
// DRAM regions that hold enclave metadata can be freed directly by calling
// free_dram_region(). Calling block_dram_region() on them will fail.
api_result_t sm_region_metadata_create (region_id_t dram_region);

// Returns the number of addressable metadata pages in a DRAM metadata region.
//
// This may be smaller than the number of total pages in a DRAM region, if the
// computer does not have continuous DRAM regions and the security monitor does
// not support using non-continuous regions.
uint64_t sm_region_metadata_pages(void);

// Returns the first usable metadata page in a DRAM metadata region.
//
// The beginning of each DRAM metadata region is reserved for the monitor's
// use. This returns the first page number that can be used to store
// enclave_info_t and thread_info_t structures.
uint64_t sm_region_metadata_start(void);

// Returns the owner of the DRAM region with the given index.
//
// Returns null_enclave_id if the given DRAM region index is invalid, locked by
// another operation, or if the region is not in the owned state.
enclave_id_t sm_region_owner (region_id_t id);

// Returns the state of the DRAM region with the given index.
//
// Returns dram_region_invalid if the given DRAM region index is invalid.
// Returns dram_region_locked if the given DRAM region is currently locked by
// another API call.
region_state_t sm_region_state (region_id_t id);


// APIs: SM Thread-related calls
// -----------------------------

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
// `entry_pc`, `entry_stack` are virtual
// addresses in the enclave's address space. They are used to set the
// corresponding fields in thread_init_info_t.
//
// 'timer_limit' is the maximum number of timer interupts and enclave can 
// receive before performing an AEX. Any unsigned integer is a valid 
// timer_limit
//
// This must be called after the enclave's root page table is set by a call to
// load_page_table().
api_result_t sm_thread_load (
  enclave_id_t enclave_id,
  thread_id_t thread_id,
  uintptr_t entry_pc,
  uintptr_t entry_stack,
  uint64_t timer_limit);

// Returns the number of pages used by a thread metadata structure.
uint64_t sm_thread_metadata_pages(void);

#endif // SECURITY_MONITOR_API_COMMON_H
