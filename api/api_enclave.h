#ifndef API_ENCLAVE_H
#define API_ENCLAVE_H

#include "api_types.h"
#include "api_util.h"

static inline api_result_t sm_exit_enclave(void) {
   return SM_API_CALL(SM_ENCLAVE_EXIT, 0, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_get_attestation_key(uintptr_t phys_addr) {
   return SM_API_CALL(SM_GET_ATTESTATION_KEY, phys_addr, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_get_public_field (public_field_t field, phys_ptr_t phys_addr) {
   return SM_API_CALL(SM_GET_PUBLIC_FIELD, field, phys_addr, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_block(region_id_t id) {
   return SM_API_CALL(SM_REGION_BLOCK, id, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_check_owned(region_id_t id) {
   return SM_API_CALL(SM_REGION_CHECK_OWNED, id, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_accept(mailbox_id_t mailbox_id, enclave_id_t expected_sender) {
   return SM_API_CALL(SM_MAIL_ACCEPT, mailbox_id, expected_sender, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_receive(mailbox_id_t mailbox_id, phys_ptr_t out_message, phys_ptr_t out_sender_measurement) {
   return SM_API_CALL(SM_MAIL_RECEIVE, mailbox_id, out_message, out_sender_measurement, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_send(enclave_id_t enclave_id, mailbox_id_t mailbox_id,
      uintptr_t phys_addr) {
   return SM_API_CALL(SM_MAIL_SEND, enclave_id, mailbox_id, phys_addr, 0, 0, 0, 0);
}

#endif // API_ENCLAVE_H
