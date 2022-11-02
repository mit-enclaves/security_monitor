#ifndef ENCLAVE_API_H
#define ENCLAVE_API_H

#define F_ADDITION 0
#define F_EXIT     1

typedef struct msg_t {
  int f;
  uintptr_t args[6];
} msg_t;


#endif // ENCLAVE_API_H
