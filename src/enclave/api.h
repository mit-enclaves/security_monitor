#ifndef SM_ENCLAVE_API_H
#define SM_ENCLAVE_API_H

#include <api_common.h>

api_result_t get_public_field (field_id_t field_id, phys_ptr_t out_field);
api_result_t get_attestation_key (phys_ptr_t out_field);

api_result_t mail_accept (enclave_id_t sender);
api_result_t mail_send (enclave_id_t recipient, phys_ptr_t in_message);
api_result_t mail_receive (phys_ptr_t out_message, phys_ptr_t out_sender_measurement);

SM_API_RESULT enclave_exit ();

#endif // SM_ENCLAVE_API_H
