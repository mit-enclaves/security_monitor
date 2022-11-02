#ifndef CRYPTO_ENCLAVE_UTIL_H
#define CRYPTO_ENCLAVE_UTIL_H

#define F_ADDITION             0
#define F_ONETIMEAUTH          1
#define F_ONETIMEAUTH_VERIF    2
#define F_SCALARMULT           3
#define F_SCALARMULT_BASE      4
#define F_STREAM_SALSA20       5
#define F_STREAM_SALSA20_XOR   6
#define F_STREAM_SALSA208      7
#define F_STREAM_SALSA208_XOR  8
#define F_STREAM_SALSA2012     9
#define F_STREAM_SALSA2012_XOR 10
#define F_STREAM_XSALSA20      11
#define F_STREAM_XSALSA20_XOR  12
#define F_VERIFY_32            13
#define F_VERIFY_16            14
#define F_SECRETBOX            15
#define F_SECRETBOX_OPEN       16
#define F_BOX                  17
#define F_BOX_OPEN             18
#define F_EXIT                 20

typedef struct msg_t {
  int f;
  uintptr_t args[6];
} msg_t;

#endif // CRYPTO_ENCLAVE_UTIL_H
