#include <crypto_enclave_api.h>
#include <os_util.h>
#include <msgq.h>

int crypto_onetimeauth(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *ki) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_ONETIMEAUTH;
  msg->args[1] = (uintptr_t) out;
  msg->args[2] = (uintptr_t) in;
  msg->args[3] = (uintptr_t) inlen;
  msg->args[4] = (uintptr_t) ki;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_onetimeauth_verify(const unsigned char *h,const unsigned char *in,unsigned long long inlen,const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_ONETIMEAUTH_VERIF;
  msg->args[1] = (uintptr_t) h;
  msg->args[2] = (uintptr_t) in;
  msg->args[3] = (uintptr_t) inlen;
  msg->args[4] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_scalarmult(unsigned char *q, const unsigned char *n, const unsigned char *p) {
  queue_t *qe = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SCALARMULT;
  msg->args[1] = (uintptr_t) q;
  msg->args[2] = (uintptr_t) n;
  msg->args[3] = (uintptr_t) p;
  int ret = 1;
  while(ret != 0) {
    ret = push(qe, msg);
  }
  return 0;
}

int crypto_scalarmult_base(unsigned char *q, const unsigned char *n) {
  queue_t *qe = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SCALARMULT_BASE;
  msg->args[1] = (uintptr_t) q;
  msg->args[2] = (uintptr_t) n;
  int ret = 1;
  while(ret != 0) {
    ret = push(qe, msg);
  }
  return 0;
}

int crypto_stream_salsa20(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA20;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) clen;
  msg->args[3] = (uintptr_t) n;
  msg->args[4] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_salsa20_xor(unsigned char *c, const unsigned char *m,unsigned long long mlen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA20_XOR;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_salsa208(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA208;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) clen;
  msg->args[3] = (uintptr_t) n;
  msg->args[4] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_salsa208_xor(unsigned char *c, const unsigned char *m,unsigned long long mlen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA208_XOR;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_salsa2012(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA2012;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) clen;
  msg->args[3] = (uintptr_t) n;
  msg->args[4] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_salsa2012_xor(unsigned char *c, const unsigned char *m,unsigned long long mlen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_SALSA2012_XOR;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_xsalsa20(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_XSALSA20;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) clen;
  msg->args[3] = (uintptr_t) n;
  msg->args[4] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_stream_xsalsa20_xor(unsigned char *c, const unsigned char *m,unsigned long long mlen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_STREAM_XSALSA20_XOR;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_verify_32(const unsigned char* a0, const unsigned char* a1) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_VERIFY_32;
  msg->args[1] = (uintptr_t) a0;
  msg->args[2] = (uintptr_t) a1;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_verify_16(const unsigned char* a0, const unsigned char* a1) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_VERIFY_16;
  msg->args[1] = (uintptr_t) a0;
  msg->args[2] = (uintptr_t) a1;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_secretbox(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SECRETBOX;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_secretbox_open(unsigned char *m, const unsigned char *c,unsigned long long clen, const unsigned char *n, const unsigned char *k) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_SECRETBOX_OPEN;
  msg->args[1] = (uintptr_t) m;
  msg->args[2] = (uintptr_t) c;
  msg->args[3] = (uintptr_t) clen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) k;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_box(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *pk, const unsigned char *sk) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_BOX;
  msg->args[1] = (uintptr_t) c;
  msg->args[2] = (uintptr_t) m;
  msg->args[3] = (uintptr_t) mlen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) pk;
  msg->args[6] = (uintptr_t) sk;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}

int crypto_box_open(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *pk, const unsigned char *sk) {
  queue_t *q = SHARED_QUEUE;  
  msg_t *msg = malloc(sizeof(msg_t));
  msg->f = F_BOX_OPEN;
  msg->args[1] = (uintptr_t) m;
  msg->args[2] = (uintptr_t) c;
  msg->args[3] = (uintptr_t) clen;
  msg->args[4] = (uintptr_t) n;
  msg->args[5] = (uintptr_t) pk;
  msg->args[6] = (uintptr_t) sk;
  int ret = 1;
  while(ret != 0) {
    ret = push(q, msg);
  }
  return 0;
}
