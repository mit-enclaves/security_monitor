#include <api_enclave.h>
#include <libnacl.h>
#include <msgq.h>
#include <crypto_enclave_util.h>

#define SHARED_MEM_REG (0x8a000000)

void enclave_entry() {
  queue_t * q = (queue_t *) SHARED_MEM_REG;
  msg_t *m;
  int ret;

  while(true) {
    ret = pop(q, (void **) &m);
    if(ret != 0) continue;
    switch((m)->f) {
      case F_ADDITION:
        m->args[0] += m->args[1];
        break;
      case F_ONETIMEAUTH:
        m->args[0] = crypto_onetimeauth(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_ONETIMEAUTH_VERIF:
        m->args[0] = crypto_onetimeauth_verify(
            (const unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_SCALARMULT:
        m->args[0] = crypto_scalarmult(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (const unsigned char *) m->args[3]);
        break;
      case F_SCALARMULT_BASE:
        m->args[0] = crypto_scalarmult_base(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2]);
        break;
      case F_STREAM_SALSA20:
        m->args[0] = crypto_stream_salsa20(
            (unsigned char *) m->args[1],
            (unsigned long long) m->args[2],
            (const unsigned char *) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_STREAM_SALSA20_XOR:
        m->args[0] = crypto_stream_salsa20_xor(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_STREAM_SALSA208:
        m->args[0] = crypto_stream_salsa208(
            (unsigned char *) m->args[1],
            (unsigned long long) m->args[2],
            (const unsigned char *) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_STREAM_SALSA208_XOR:
        m->args[0] = crypto_stream_salsa208_xor(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_STREAM_SALSA2012:
        m->args[0] = crypto_stream_salsa2012(
            (unsigned char *) m->args[1],
            (unsigned long long) m->args[2],
            (const unsigned char *) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_STREAM_SALSA2012_XOR:
        m->args[0] = crypto_stream_salsa2012_xor(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_STREAM_XSALSA20:
        m->args[0] = crypto_stream_xsalsa20(
            (unsigned char *) m->args[1],
            (unsigned long long) m->args[2],
            (const unsigned char *) m->args[3],
            (const unsigned char *) m->args[4]);
        break;
      case F_STREAM_XSALSA20_XOR:
        m->args[0] = crypto_stream_xsalsa20_xor(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_VERIFY_32:
        m->args[0] = crypto_verify_32(
            (const unsigned char *) m->args[1],
            (const unsigned char *) m->args[2]);
        break;
      case F_VERIFY_16:
        m->args[0] = crypto_verify_16(
            (const unsigned char *) m->args[1],
            (const unsigned char *) m->args[2]);
        break;
      case F_SECRETBOX:
        m->args[0] = crypto_secretbox(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_SECRETBOX_OPEN:
        m->args[0] = crypto_secretbox_open(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5]);
        break;
      case F_BOX:
        m->args[0] = crypto_box(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5],
            (const unsigned char *) m->args[6]);
        break;
      case F_BOX_OPEN:
        m->args[0] = crypto_box_open(
            (unsigned char *) m->args[1],
            (const unsigned char *) m->args[2],
            (unsigned long long) m->args[3],
            (const unsigned char *) m->args[4],
            (const unsigned char *) m->args[5],
            (const unsigned char *) m->args[6]);
        break;
      case F_EXIT:
        sm_exit_enclave();
      default:
        break;
    } 
  }
}
