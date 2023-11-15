#ifndef SM_HANDLES_H
#define SM_HANDLES_H

#include "api_types.h"

// Handles for SM API
api_result_t sm_internal_enclave_exit ();

api_result_t sm_internal_enclave_get_keys (uintptr_t addr_m, uintptr_t addr_pk, uintptr_t addr_sk, uintptr_t addr_a);

api_result_t sm_internal_get_public_field (public_field_t field, uintptr_t addr);

api_result_t sm_internal_region_check_owned (region_id_t id);

api_result_t sm_internal_region_block (region_id_t id);

api_result_t sm_internal_region_update ();

api_result_t sm_internal_mail_accept (mailbox_id_t mailbox_id, enclave_id_t expected_sender);

api_result_t sm_internal_mail_receive (mailbox_id_t mailbox_id, uintptr_t out_message, uintptr_t out_sender_measurement);

api_result_t sm_internal_mail_send (enclave_id_t enclave_id, mailbox_id_t mailbox_id, uintptr_t addr);

// Internal SM usage  
api_result_t sm_internal_perform_enclave_exit(bool aex_present);

void sm_internal_enclave_aex (uintptr_t *regs);

#endif // API_TYPES_H
