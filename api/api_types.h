#ifndef API_TYPES_H
#define API_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <api_crypto_types.h>

// SM API Types
// --------------------

typedef unsigned long long phys_ptr_t;
typedef phys_ptr_t enclave_id_t;
typedef phys_ptr_t thread_id_t;
typedef unsigned long long region_id_t;
typedef unsigned long long  mailbox_id_t;

// reserved, "special" enclave_id values. These must not be valid (this implementation requires enclave_id_t be page-aligned, so these values are safe).
#define OWNER_UNTRUSTED (1)
#define OWNER_SM        (2)

// reserved, "special" thread_id values. These must not be valid.
#define ENCLAVE_THREAD_NONE (1)

// ### SM API Return values
typedef enum {
   // API call succeeded.
   MONITOR_OK = 0,

   // A parameter given to the API call was invalid.
   //
   // This most likely reflects a bug in the caller code.
   MONITOR_INVALID_VALUE = 1,

   // A resource referenced by the API call is in an unsuitable state.
   //
   // The API call will not succeed if the caller simply retries it. However,
   // the caller may be able to perform other API calls to get the resources in
   // a state that will allow this call to succeed.
   MONITOR_INVALID_STATE = 2,

   // Failed to acquire a lock. Retrying with the same arguments might succeed.
   //
   // The monitor returns this instead of blocking a hardware thread when a
   // resource lock is acquired by another thread. This approach eliminates any
   // possibility of having the monitor deadlock. The caller is responsible for
   // retrying the API call.
   //
   // This is also sometime returned instead of monitor_invalid_value, in the
   // interest of reducing edge cases in monitor implementation.
   MONITOR_CONCURRENT_CALL = 3,

   // The call was interrupted due to an asynchronous enclave exit (AEX).
   //
   // This is only returned by enter_enclave, and can be considered a more
   // specific case of monitor_concurrent_call. The caller should retry the
   // enclave_enter call, so the enclave thread can make progress.
   MONITOR_ASYNC_EXIT = 4,

   // The caller is not allowed to access a resource referenced by the API call.
   //
   // This is a more specific version of monitor_invalid_value. The monitor does
   // its best to identify these cases, but may fail.
   MONITOR_ACCESS_DENIED = 5,

   // The current monitor implementation does not support the request.
   //
   // The caller made a reasonable API request that exercises an unhandled edge
   // case in the monitor implementaiton. Some edge cases that would require
   // complex or difficult-to-test implementations are detected and handled by
   // returning monitor_unsupported.
   //
   // The documentation for API calls states the edge cases that result in a
   // monitor_unsupported response.
   MONITOR_UNSUPPORTED = 6,
} api_result_t;

typedef enum {
  PUBLIC_FIELD_PK_M = 0,
  PUBLIC_FIELD_PK_D = 1,
  PUBLIC_FIELD_PK_SM = 2,
  PUBLIC_FIELD_H_SM = 3,
  PUBLIC_FIELD_SIG_D = 4,
  PUBLIC_FIELD_SIG_SM = 5,
} public_field_t;

typedef enum {
  REGION_STATE_INVALID = 0,
  REGION_STATE_FREE = 1,
  REGION_STATE_OWNED = 2,
  REGION_STATE_BLOCKED = 3,
} region_state_t;

typedef enum {
  REGION_TYPE_SM = 0,
  REGION_TYPE_UNTRUSTED = 0,
  REGION_TYPE_ENCLAVE = 1,
  REGION_TYPE_METADATA = 2,
} region_type_t;

typedef struct {
  uint8_t lgsizes[64]; // TODO: fix hardcoding here...
} cache_partition_t;

// SM API Syscall codes
// --------------------
#define SM_ENCLAVE_CREATE                   (1000)
#define SM_ENCLAVE_DELETE                   (1001)
#define SM_ENCLAVE_ENTER                    (1002)
#define SM_ENCLAVE_EXIT                     (1003)
#define SM_ENCLAVE_INIT                     (1004)
#define SM_ENCLAVE_LOAD_HANDLER             (1005)
#define SM_ENCLAVE_LOAD_PAGE_TABLE          (1006)
#define SM_ENCLAVE_LOAD_PAGE                (1007)
#define SM_ENCLAVE_METADATA_PAGES           (1008)
#define SM_ENCLAVE_GET_ATTEST               (1009)

#define SM_ENCLAVE_GET_KEYS                 (1010)
#define SM_GET_PUBLIC_FIELD                 (1011)

#define SM_MAIL_ACCEPT                      (1020)
#define SM_MAIL_RECEIVE                     (1021)
#define SM_MAIL_SEND                        (1022)

#define SM_REGION_ASSIGN                    (1030)
#define SM_REGION_BLOCK                     (1031)
#define SM_REGION_CHECK_OWNED               (1032)
#define SM_REGION_UPDATE                    (1033)
#define SM_REGION_FREE                      (1034)
#define SM_REGION_METADATA_CREATE           (1035)
#define SM_REGION_METADATA_PAGES            (1036)
#define SM_REGION_METADATA_START            (1037)
#define SM_REGION_OWNER                     (1038)
#define SM_REGION_STATE                     (1039)
#define SM_REGION_CACHE_PART                (1040)

#define SM_THREAD_DELETE                    (1050)
#define SM_THREAD_LOAD                      (1051)
#define SM_THREAD_METADATA_PAGES            (1052)

#endif // API_TYPE_H
