#ifndef API_ENCLAVE_H
#define API_ENCLAVE_H

#include "api_types.h"
#include "api_util.h"

static inline api_result_t sm_exit_enclave(void) {
   return SM_API_CALL(SM_ENCLAVE_EXIT, 0, 0, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_enclave_get_keys (void *addr_m, void *addr_pk, void *addr_sk, void *addr_a) {
  return SM_API_CALL(SM_ENCLAVE_GET_KEYS, addr_m, addr_pk, addr_sk, addr_a, 0, 0, 0);
}

static inline api_result_t sm_get_public_field (public_field_t field, void *addr) {
   return SM_API_CALL(SM_GET_PUBLIC_FIELD, field, addr, 0, 0, 0, 0, 0);
}

static inline api_result_t sm_region_update () {
  return SM_API_CALL(SM_REGION_UPDATE, 0, 0, 0, 0, 0, 0, 0);
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

static inline api_result_t sm_mail_receive(mailbox_id_t mailbox_id, void *out_message, void *out_sender_measurement) {
   return SM_API_CALL(SM_MAIL_RECEIVE, mailbox_id, out_message, out_sender_measurement, 0, 0, 0, 0);
}

static inline api_result_t sm_mail_send(enclave_id_t enclave_id, mailbox_id_t mailbox_id, void * addr) {
   return SM_API_CALL(SM_MAIL_SEND, enclave_id, mailbox_id, addr, 0, 0, 0, 0);
}

#endif // API_ENCLAVE_H
