#include <libnacl.h>

// Instead of both being out-parameters as in the original NaCl library
// the sk is a in-param where the user has to provide a sufficiently random number
int crypto_box_keypair(
  unsigned char *pk,
  unsigned char *sk
)
{
  return crypto_scalarmult_base(pk,sk);
}
