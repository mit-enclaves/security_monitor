#ifndef SECURITY_MONITOR_API_H
#define SECURITY_MONITOR_API_H
#include <data_structures.h>

//ECALL codes for SM-calls

// SM CALLS FROM ENCLAVE (from U-mode, within an enclave)
#define UBI_SM_ENCLAVE_BLOCK_DRAM_REGION      1000

#define UBI_SM_ENCLAVE_CHECK_OWNERSHIP        1001

#define UBI_SM_ENCLAVE_ACCEPT_THREAD          1002

#define UBI_SM_ENCLAVE_EXIT_ENCLAVE           1003

#define UBI_SM_ENCLAVE_GET_ATTESTATION_KEY    1004

#define UBI_SM_ENCLAVE_ACCEPT_MESSAGE         1005
#define UBI_SM_ENCLAVE_READ_MESSAGE           1006
#define UBI_SM_ENCLAVE_SEND_MESSAGE           1007

// SM CALLS FROM OS (these come from S-mode)
#define SBI_SM_OS_BLOCK_DRAM_REGION           2000

#define SBI_SM_OS_SET_DMA_RANGE               2001

#define SBI_SM_OS_DRAM_REGION_STATE           2002
#define SBI_SM_OS_DRAM_REGION_OWNER           2003
#define SBI_SM_OS_ASSIGN_DRAM_REGION          2004
#define SBI_SM_OS_FREE_DRAM_REGION            2005
#define SBI_SM_OS_FLUSH_CACHED_DRAM_REGIONS   2006

#define SBI_SM_OS_CREATE_METADATA_REGION      2007
#define SBI_SM_OS_METADATA_REGION_PAGES       2008
#define SBI_SM_OS_METADATA_REGION_START       2009
#define SBI_SM_OS_THREAD_METADATA_PAGES       2010
#define SBI_SM_OS_ENCLAVE_METADATA_PAGES      2011

#define SBI_SM_OS_CREATE_ENCLAVE              2012
#define SBI_SM_OS_LOAD_PAGE_TABLE             2013
#define SBI_SM_OS_LOAD_PAGE                   2014
#define SBI_SM_OS_LOAD_THREAD                 2015
#define SBI_SM_OS_ASSIGN_THREAD               2016
#define SBI_SM_OS_INIT_ENCLAVE                2017

#define SBI_SM_OS_ENTER_ENCLAVE               2018

#define SBI_SM_OS_DELETE_THREAD               2019

#define SBI_SM_OS_DELETE_ENCLAVE              2020

#define SBI_SM_OS_COPY_DEBUG_ENCLAVE_PAGE     2021

#define SBI_SM_ENCLAVE_FETCH_FIELD            2022

// Error codes returned from monitor API calls.
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
api_result_t os_block_dram_region(dram_region_id_t id);
api_result_t enclave_block_dram_region(dram_region_id_t id);

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
api_result_t create_metadata_region(size_t dram_region);

// Returns the number of addressable metadata pages in a DRAM metadata region.
//
// This may be smaller than the number of total pages in a DRAM region, if the
// computer does not have continuous DRAM regions and the security monitor does
// not support using non-continuous regions.
size_t metadata_region_pages();

// Returns the first usable metadata page in a DRAM metadata region.
//
// The beginning of each DRAM metadata region is reserved for the monitor's
// use. This returns the first page number that can be used to store
// enclave_info_t and thread_info_t structures.
size_t metadata_region_start();

// Returns the number of pages used by a thread metadata structure.
size_t thread_metadata_pages();

// Returns the number of pages used by an enclave metadata structure.
size_t enclave_metadata_pages(size_t mailbox_count);

#endif // SECURITY_MONITOR_API_H
