#include <libnacl.h>

static const unsigned char sigma[16] = "expand 32-byte k";
static const unsigned char n[16] = {0};

int crypto_box_beforenm(
  unsigned char *k,
  const unsigned char *pk,
  const unsigned char *sk
)
{
  unsigned char s[32];
  crypto_scalarmult(s,sk,pk);
  crypto_core_hsalsa20(k,n,s,sigma);
  return 0;
}
