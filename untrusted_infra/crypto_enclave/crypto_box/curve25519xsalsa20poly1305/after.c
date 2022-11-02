#include <libnacl.h>

int crypto_box_afternm(
  unsigned char *c,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  return crypto_secretbox(c,m,mlen,n,k);
}

int crypto_box_open_afternm(
  unsigned char *m,
  const unsigned char *c,unsigned long long clen,
  const unsigned char *n,
  const unsigned char *k
)
{
  return crypto_secretbox_open(m,c,clen,n,k);
}
