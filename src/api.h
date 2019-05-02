#ifndef SECURITY_MONITOR_API_H
#define SECURITY_MONITOR_API_H
#include <data_structures.h>

//// DRAM

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
api_result_t block_dram_region(dram_region_id_t id);

//namespace enclave {  // sanctum::api::enclave

// Returns monitor_ok if the given DRAM region is owned by the calling enclave.
//
// This is used by enclaves to confirm that they own a DRAM region when the OS
// tells them that they do. The enclave should that assume something went wrong
// if it sees any return value other than monitor_ok.
api_result_t dram_region_check_ownership(dram_region_id_t id);

// Returns the state of the DRAM region with the given index.
//
// Returns dram_region_invalid if the given DRAM region index is invalid.
// Returns dram_region_locked if the given DRAM region is currently locked by
// another API call.
dram_region_state_t dram_region_state(dram_region_id_t id);

// Returns the owner of the DRAM region with the given index.
//
// Returns null_enclave_id if the given DRAM region index is invalid, locked by
// another operation, or if the region is not in the owned state.
enclave_id_t dram_region_owner(dram_region_id_t id);

// Assigns a free DRAM region to an enclave or to the OS.
//
// `new_owner` is the enclave ID of the enclave that will own the DRAM region.
// 0 means that the DRAM region will be assigned to the OS.
//
api_result_t assign_dram_region(dram_region_id_t id, enclave_id_t new_owner);

// Frees a DRAM region that was previously locked.
api_result_t free_dram_region(dram_region_id_t id);

// Performs the TLB flushes needed to free a locked region.
//
// System software must invoke this call instead of flushing the TLB directly,
// as the monitor's state must be updated to reflect the fact that a TLB flush
// has occurred.
api_result_t flush_cached_dram_regions();

//// MAILBOXES

// Reads the monitor's private attestation key.
//
// This API call will only succeed if the calling enclave is the special
// enclave designated by the security monitor to be the signing enclave.
//
// `phys_addr` must point into a buffer large enough to store the attestation
// key. The entire buffer must be contained in a single DRAM region that
// belongs to the enclave.
api_result_t get_attestation_key(uintptr_t phys_addr);

// Prepares a mailbox to receive a message from another enclave.
//
// The mailbox will discard any message that it might contain.
api_result_t accept_message(mailbox_id_t mailbox_id, enclave_id_t expected_sender);

// Attempts to read a message received in a mailbox.
//
// If the read succeeds, the mailbox will transition into the free state.
//
// `phys_addr` must point into a buffer large enough to store a
// mailbox_identity_t structure. The entire buffer must be contained in a
// single DRAM region that belongs to the enclave.
api_result_t read_message(mailbox_id_t mailbox_id, uintptr_t phys_addr);

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
api_result_t send_message(enclave_id_t enclave_id, mailbox_id_t mailbox_id,
      uintptr_t phys_addr);

//// METADATA

// Reserves a free DRAM region to hold enclave metadata.
//
// DRAM regions that hold enclave metadata can be freed directly by calling
// free_dram_region(). Calling block_dram_region() on them will fail.
api_result_t create_metadata_region(dram_region_id_t dram_region);

// Returns the number of addressable metadata pages in a DRAM metadata region.
//
// This may be smaller than the number of total pages in a DRAM region, if the
// computer does not have continuous DRAM regions and the security monitor does
// not support using non-continuous regions.
uint64_t metadata_region_pages();

// Returns the first usable metadata page in a DRAM metadata region.
//
// The beginning of each DRAM metadata region is reserved for the monitor's
// use. This returns the first page number that can be used to store
// enclave_info_t and thread_info_t structures.
uint64_t metadata_region_start();

// Returns the number of pages used by a thread metadata structure.
uint64_t thread_metadata_pages();

// Returns the number of pages used by an enclave metadata structure.
uint64_t enclave_metadata_pages(uint64_t mailbox_count);

//// ENCLAVE MANAGEMENT

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
api_result_t create_enclave(enclave_id_t enclave_id, uintptr_t ev_base,
      uintptr_t ev_mask, uint64_t mailbox_count, bool debug);

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

// Marks the given enclave as initialized and ready to execute.
//
// `enclave_id` must identify an enclave that has not yet been initialized.
api_result_t init_enclave(enclave_id_t enclave_id);

// Frees up all DRAM regions and the metadata associated with an enclave.
//
// This can only be called when there is no thread metadata associated with the
// enclave.
api_result_t delete_enclave(enclave_id_t enclave_id);

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
api_result_t enter_enclave(enclave_id_t enclave_id, thread_id_t thread_id);

// Ends the currently running enclave thread and returns control to the OS.
api_result_t exit_enclave();

//// THREAD MANAGEMENT

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
api_result_t load_thread(enclave_id_t enclave_id, thread_id_t thread_id,
    uintptr_t entry_pc, uintptr_t entry_stack, uintptr_t fault_pc,
    uintptr_t fault_stack);

// Allocates a thread metadata structure to be used by an enclave.
//
// `enclave_id` must be an enclave that has been initialized and has not yet
// been killed.
//
// `thread_id` must be the physical address of the first page in a sequence of
// free pages in the same DRAM metadata region. It becomes the thread's ID used
// for subsequent API calls. The required number of free metadata pages can be
// obtained by calling `thread_metadata_pages`.
api_result_t assign_thread(enclave_id_t enclave_id, thread_id_t thread_id);

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
api_result_t accept_thread(thread_id_t thread_id, uintptr_t thread_info_addr);

// Deallocates a thread info slot.
//
// The thread must not be running on any core.
api_result_t delete_thread(thread_id_t thread_id);

#endif // SECURITY_MONITOR_API_H
