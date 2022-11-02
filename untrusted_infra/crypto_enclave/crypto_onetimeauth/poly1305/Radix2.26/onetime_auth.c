#include "stdint.h"
extern void addasm(unsigned int h[17], const unsigned int c[17]);
extern void add226asm(unsigned int h[5], const unsigned int c[5]);
extern void add226asm_wo_squeeze(unsigned int h[5], const unsigned int c[5]);
extern void toradix28asm(unsigned int h[17]);
extern void squeeze226asm(unsigned int h[5]);
extern void mulmod226asm(unsigned int h[5], unsigned int r[5]);
extern int onetimeauth_loop(const unsigned char *in , int inlen, unsigned int *h,
                      unsigned int *r, unsigned int *c);


static const unsigned int minusp[5] = {5, 0, 0, 0, 0x4000000};

//static const unsigned int minusp[17] = {5, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 252};

static void freeze(unsigned int h[5]) {
  unsigned int horig[5];
  unsigned int j;
  unsigned int negative;
  for (j = 0; j < 5; ++j)
    horig[j] = h[j];
  add226asm_wo_squeeze(h, minusp);
  negative = -(h[4] >> 26);
  for (j = 0; j < 5; ++j)
    h[j] ^= negative & (horig[j] ^ h[j]);
}

void toradix28_130(unsigned int h[17]) {
  h[16] = (h[4] >> 24);
  h[15] = (h[4] >> 16) & 0xFF;
  h[14] = (h[4] >> 8) & 0xFF;
  h[13] = h[4] & 0xFF;
  h[12] = (h[3] >> 18) & 0xFF;
  h[11] = (h[3] >> 10) & 0xFF;
  h[10] = (h[3] >> 2) & 0xFF;
  h[9] = (h[2] >> 20) + ((h[3] & 3) << 6);
  h[8] = (h[2] >> 12) & 0xFF;
  h[7] = (h[2] >> 4) & 0xFF;
  h[6] = (h[1] >> 22) + ((h[2] & 0x0F) << 4);
  h[5] = (h[1] >> 14) & 0xFF;
  h[4] = (h[1] >> 6) & 0xFF;
  h[3] = (h[0] >> 24) + ((h[1] & 0x3f) << 2);
  h[2] = (h[0] >> 16) & 0xFF;
  h[1] = (h[0] >> 8) & 0xFF;
  h[0] = h[0] & 0xFF;
}

// input is in little endian
int crypto_onetimeauth(unsigned char *out, const unsigned char *in,
                       unsigned long long inlen, const unsigned char *k) {
  unsigned int j;
  unsigned int r[5];
  unsigned int h[17];
  unsigned int c[17];

  // create R from the first 16 bytes of the key
  r[0] = k[0] + (k[1] << 8) + (k[2] << 16) + ((k[3] & 3) << 24);
  r[1] = ((k[3] >> 2) & 3) + ((k[4] & 252) << 6) + (k[5] << 14) +
         ((k[6] & 15) << 22);
  r[2] = (k[6] >> 4) + ((k[7] & 15) << 4) + ((k[8] & 252) << 12) +
         ((k[9] & 63) << 20);
  r[3] =
      (k[9] >> 6) + (k[10] << 2) + ((k[11] & 15) << 10) + ((k[12] & 252) << 18);
  r[4] = k[13] + (k[14] << 8) + ((k[15] & 15) << 16);

  // set the state to 0
  for (j = 0; j < 17; ++j)
    h[j] = 0;

  // do the bulk of the authloop
  int newinlen = onetimeauth_loop(in, inlen, h, r, c);

  // calculate how much work is left (always less than 16 bytes)
  in += inlen - newinlen;
  inlen = newinlen;

  if (newinlen != 0) {

    // do the remaining work
    for (j = 0; j < 5; ++j)
      c[j] = 0; // set c to 0
    int index = 0;
    int bitleft = 26;
    for (j = 0; (j < 16) && (j < newinlen); ++j) {
      if (bitleft < 8) {
        int tmp = ((1 << bitleft) - 1);
        c[index] += (in[j] & tmp) << (26 - bitleft);
        index++;
        c[index] += in[j] >> bitleft;
        bitleft = 26 - (8 - bitleft);
      } else {
        c[index] +=
            in[j]
            << (26 -
                bitleft); // fill c with a chunk of 16 bytes from the in param
        bitleft -= 8;
      }
    }
    if (bitleft == 0) {
      index++;
      bitleft = 26;
    }
    c[index] += 1 << (26 - bitleft);

    add226asm(h, c);    // c to the state
    mulmod226asm(h, r); // multiply state with the secret key modulo 2^130-5
  }

  // go back to radix 2.8

  freeze(h); // calculate mod 2^130-5
  toradix28_130(h);

  for (j = 0; j < 16; ++j)
    c[j] = k[j + 16];
  c[16] = 0;
  addasm(h, c); // add S to the state (which is the last 16 bytes of the key)
  for (j = 0; j < 16; ++j)
    out[j] = h[j]; // output the state modulo 2^128 (the last 16 bytes)
  return 0;
}
