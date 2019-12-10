#ifndef SM_ENCLAVE_API_H
#define SM_ENCLAVE_API_H

#include <common/api.h>

SM_API_RESULT get_public_field(field_id_t field_id, phys_ptr_t out_field);
SM_API_RESULT get_attestation_key(phys_ptr_t out_field);

SM_API_RESULT accept_mail(enclave_id_t sender);
SM_API_RESULT send_mail(enclave_id_t recipient, phys_ptr_t in_message);
SM_API_RESULT get_mail(phys_ptr_t out_message, phys_ptr_t out_sender_measurement);

SM_API_RESULT enclave_exit();

#endif // SM_ENCLAVE_API_H
