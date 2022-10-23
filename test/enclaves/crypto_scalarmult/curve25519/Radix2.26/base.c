#include <libnacl.h>

const unsigned char base[32] = {9};

int crypto_scalarmult_base(unsigned char *q,
  const unsigned char *n)
{
  return crypto_scalarmult(q,n,base);
}
