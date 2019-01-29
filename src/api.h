#ifndef SECURITY_MONITOR_API_H
#define SECURITY_MONITOR_API_H

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
api_result_t block_dram_region(size_t dram_region);

//namespace enclave {  // sanctum::api::enclave

// Returns monitor_ok if the given DRAM region is owned by the calling enclave.
//
// This is used by enclaves to confirm that they own a DRAM region when the OS
// tells them that they do. The enclave should that assume something went wrong
// if it sees any return value other than monitor_ok.
api_result_t dram_region_check_ownership(size_t dram_region);

// Reads the monitor's private attestation key.
//
// This API call will only succeed if the calling enclave is the special
// enclave designated by the security monitor to be the signing enclave.
//
// `phys_addr` must point into a buffer large enough to store the attestation
// key. The entire buffer must be contained in a single DRAM region that
// belongs to the enclave.
api_result_t get_attestation_key(uintptr_t phys_addr);

// The size of enclave's measurement, in bytes.
#define measurement_size 64

//namespace os {  // sanctum::api::os

// Per-DRAM region accounting information.
typedef enum {
  dram_region_invalid = 0,
  dram_region_free = 1,
  dram_region_blocked = 2,
  dram_region_locked = 3,
  dram_region_owned = 4,
} dram_region_state_t;

// Returns the state of the DRAM region with the given index.
//
// Returns dram_region_invalid if the given DRAM region index is invalid.
// Returns dram_region_locked if the given DRAM region is currently locked by
// another API call.
dram_region_state_t dram_region_state(size_t dram_region);

// Returns the owner of the DRAM region with the given index.
//
// Returns null_enclave_id if the given DRAM region index is invalid, locked by
// another operation, or if the region is not in the owned state.
enclave_id_t dram_region_owner(size_t dram_region);

// Assigns a free DRAM region to an enclave or to the OS.
//
// `new_owner` is the enclave ID of the enclave that will own the DRAM region.
// 0 means that the DRAM region will be assigned to the OS.
//
api_result_t assign_dram_region(size_t dram_region, enclave_id_t new_owner);

// Frees a DRAM region that was previously locked.
api_result_t free_dram_region(size_t dram_region);

// Performs the TLB flushes needed to free a locked region.
//
// System software must invoke this call instead of flushing the TLB directly,
// as the monitor's state must be updated to reflect the fact that a TLB flush
// has occurred.
api_result_t flush_cached_dram_regions();

#endif // SECURITY_MONITOR_API_H
