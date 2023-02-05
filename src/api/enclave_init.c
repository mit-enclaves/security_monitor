#include <sm.h>

api_result_t sm_internal_enclave_init (enclave_id_t enclave_id) {

  // Caller is authenticated and authorized by the trap routing logic : the trap handler and MCAUSE unambiguously identify the caller, and the trap handler does not route unauthorized API calls.

  // Validate inputs
  // ---------------

  /*
    - enclave_id must point to a valid enclave such that:
      - the enclave has had its handler, page tables, and at least 1 page of data loded.
  */

  region_map_t locked_regions = (const region_map_t){ 0 };

  // <TRANSACTION>
  api_result_t result = add_lock_region_iff_valid_enclave(enclave_id, &locked_regions);
  if ( MONITOR_OK != result ) {
    return result;
  }

  enclave_metadata_t * enclave_metadata = (enclave_metadata_t *)(enclave_id);

  // Make sure the enclave is in correct state:
  if (enclave_metadata->init_state != ENCLAVE_STATE_PAGE_DATA_LOADED) {
    unlock_regions(&locked_regions);
    return MONITOR_INVALID_STATE;
  }

  // NOTE: Inputs are now deemed valid.

  // Apply state transition
  // ----------------------
  sm_state_t * sm = get_sm_state_ptr();
  public_key_t * pk = &(sm->keys.software_public_key);
  secret_key_t * sk = &(sm->keys.software_secret_key);

  // Finalize enclave measurement
  hash_finalize( &enclave_metadata->hash_context, &enclave_metadata->measurement );

  // Provision the enclave's keys
  hash_context_t h_ctxt_s;
  hash_init(&h_ctxt_s);
  hash_extend(&h_ctxt_s, sk, sizeof(secret_key_t));
  hash_extend(&h_ctxt_s, &enclave_metadata->measurement, sizeof(hash_t));

  hash_t key_seed;
  hash_finalize(&h_ctxt_s, &key_seed);

  create_secret_signing_key((key_seed_t *) &key_seed, &enclave_metadata->secret_key);
  compute_public_signing_key(&enclave_metadata->secret_key, &enclave_metadata->public_key);

  // Generate The Attestation Certificate
  hash_context_t h_ctxt_a;
  hash_init(&h_ctxt_a);
  hash_extend(&h_ctxt_a, &enclave_metadata->measurement, sizeof(hash_t));
  hash_extend(&h_ctxt_a, &enclave_metadata->public_key, sizeof(public_key_t));

  hash_t attestation;
  hash_finalize(&h_ctxt_a, &attestation);

  sign(&attestation, sizeof(hash_t), pk, sk, &enclave_metadata->attestation);

  // Initialize Enclave
  enclave_metadata->init_state = ENCLAVE_STATE_INITIALIZED;

  // Release locks
  unlock_regions(&locked_regions);
  // </TRANSACTION>

  // TODO bonus security clean the cache and flush L2
  return MONITOR_OK;
}
