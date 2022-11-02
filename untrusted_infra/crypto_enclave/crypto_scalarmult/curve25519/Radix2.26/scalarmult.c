#include <stdio.h>
extern void karatsuba226_255(unsigned int *, const unsigned int *,
                         const unsigned int *);
extern void mainloop226_asm(unsigned int *, const unsigned int *);
extern void squeeze226_255(unsigned int *);

// addition of a and b
// possible need to squeeze for the karatsuba
void add226(unsigned int out[10], const unsigned int a[10],
            const unsigned int b[10]) {
  unsigned int j;
  unsigned int u;
  u = 0;
  for (j = 0; j < 9; ++j) {
    u += a[j] + b[j];
    out[j] = u & 0x3ffffff;
    u >>= 26;
  }
  u += a[9] + b[9];
  out[9] = u;
  squeeze226_255(out);
}

void add226_wo_squeeze(unsigned int out[10], const unsigned int a[10],
                       const unsigned int b[10]) {
  unsigned int j;
  unsigned int u;
  u = 0;
  for (j = 0; j < 9; ++j) {
    u += a[j] + b[j];
    out[j] = u & 0x3ffffff;
    u >>= 26;
  }
  u += a[9] + b[9];
  out[9] = u;
}

// subtraction of a and b
void sub226(unsigned int out[10], const unsigned int a[10],
            const unsigned int b[10]) {
  unsigned int j;
  unsigned long long u;
  u = 0x3fffda0;
  for (j = 0; j < 9; ++j) {
    u += a[j] + 0xffffffc000000 - b[j];
    out[j] = u & 0x3ffffff;
    u >>= 26;
  }
  u += a[9] - b[9];
  out[9] = u;
  squeeze226_255(out);
}
const unsigned int minusp[10] = {19, 0, 0, 0, 0, 0, 0, 0, 0, 0x200000};

void freeze(unsigned int a[10]) {
  unsigned int aorig[10];
  unsigned int j;
  unsigned int negative;

  for (j = 0; j < 10; ++j)
    aorig[j] = a[j];
  add226_wo_squeeze(a, a, minusp);
  negative = -((a[9] >> 21) & 1);
  for (j = 0; j < 10; ++j)
    a[j] ^= negative & (aorig[j] ^ a[j]);
}

void recip226(unsigned int out[10], const unsigned int z[10]) {
  unsigned int z11[10];
  unsigned int t0[10];
  unsigned int t1[10];
  unsigned int t2[10];
  int i;

  /* 2 */ karatsuba226_255(z11, z, z);
  /* 4 */ karatsuba226_255(t0, z11, z11);
  /* 8 */ karatsuba226_255(t0, t0, t0);
  /* 9 */ karatsuba226_255(t0, t0, z);
  /* 11 */ karatsuba226_255(z11, z11, t0);
  /* 22 */ karatsuba226_255(t1, z11, z11);
  /* 2^5 - 2^0 = 31 */ karatsuba226_255(t0, t1, t0);

  /* 2^6 - 2^1 */ karatsuba226_255(t1, t0, t0);
  /* 2^7 - 2^2 */ karatsuba226_255(t1, t1, t1);
  /* 2^8 - 2^3 */ karatsuba226_255(t1, t1, t1);
  /* 2^9 - 2^4 */ karatsuba226_255(t1, t1, t1);
  /* 2^10 - 2^5 */ karatsuba226_255(t1, t1, t1);
  /* 2^10 - 2^0 */ karatsuba226_255(t1, t0, t1); // last use z2_5_0

  /* 2^11 - 2^1 */ karatsuba226_255(t0, t1, t1);
  /* 2^12 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^20 - 2^10 */ for (i = 2; i < 10; i += 2) {
    karatsuba226_255(t0, t0, t0);
    karatsuba226_255(t0, t0, t0);
  }
  /* 2^20 - 2^0 */ karatsuba226_255(t0, t1, t0);

  /* 2^21 - 2^1 */ karatsuba226_255(t2, t0, t0);
  /* 2^22 - 2^2 */ karatsuba226_255(t2, t2, t2);
  /* 2^40 - 2^20 */ for (i = 2; i < 20; i += 2) {
    karatsuba226_255(t2, t2, t2);
    karatsuba226_255(t2, t2, t2);
  }
  /* 2^40 - 2^0 */ karatsuba226_255(t0, t0, t2); // last use of z2_20_0

  /* 2^41 - 2^1 */ karatsuba226_255(t0, t0, t0);
  /* 2^42 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^50 - 2^10 */ for (i = 2; i < 10; i += 2) {
    karatsuba226_255(t0, t0, t0);
    karatsuba226_255(t0, t0, t0);
  }
  /* 2^50 - 2^0 */ karatsuba226_255(t2, t0, t1); // last use z2_10_0

  /* 2^51 - 2^1 */ karatsuba226_255(t0, t2, t2);
  /* 2^52 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^100 - 2^50 */ for (i = 2; i < 50; i += 2) {
    karatsuba226_255(t0, t0, t0);
    karatsuba226_255(t0, t0, t0);
  }
  /* 2^100 - 2^0 */ karatsuba226_255(t1, t0, t2);

  /* 2^101 - 2^1 */ karatsuba226_255(t0, t1, t1);
  /* 2^102 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^200 - 2^100 */ for (i = 2; i < 100; i += 2) {
    karatsuba226_255(t0, t0, t0);
    karatsuba226_255(t0, t0, t0);
  }
  /* 2^200 - 2^0 */ karatsuba226_255(t0, t0, t1); // lst use z2_100_0

  /* 2^201 - 2^1 */ karatsuba226_255(t0, t0, t0);
  /* 2^202 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^250 - 2^50 */ for (i = 2; i < 50; i += 2) {
    karatsuba226_255(t0, t0, t0);
    karatsuba226_255(t0, t0, t0);
  }
  /* 2^250 - 2^0 */ karatsuba226_255(t0, t0, t2); // last use z2_50_0

  /* 2^251 - 2^1 */ karatsuba226_255(t0, t0, t0);
  /* 2^252 - 2^2 */ karatsuba226_255(t0, t0, t0);
  /* 2^253 - 2^3 */ karatsuba226_255(t0, t0, t0);
  /* 2^254 - 2^4 */ karatsuba226_255(t0, t0, t0);
  /* 2^255 - 2^5 */ karatsuba226_255(t0, t0, t0);
  /* 2^255 - 21 */ karatsuba226_255(out, t0, z11);
}

void convert_to_radix226_255(unsigned int *r, const unsigned char *k) {
  r[0] = k[0] + (k[1] << 8) + (k[2] << 16) + ((k[3] & 3) << 24);
  r[1] = (k[3] >> 2) + (k[4] << 6) + (k[5] << 14) + ((k[6] & 15) << 22);
  r[2] = (k[6] >> 4) + (k[7] << 4) + (k[8] << 12) + ((k[9] & 63) << 20);
  r[3] = (k[9] >> 6) + (k[10] << 2) + ((k[11]) << 10) + (k[12] << 18);
  r[4] = k[13] + (k[14] << 8) + (k[15] << 16) + ((k[16] & 3) << 24);
  r[5] = (k[16] >> 2) + (k[17] << 6) + (k[18] << 14) + ((k[19] & 15) << 22);
  r[6] = (k[19] >> 4) + (k[20] << 4) + (k[21] << 12) + ((k[22] & 63) << 20);
  r[7] = (k[22] >> 6) + (k[23] << 2) + ((k[24]) << 10) + (k[25] << 18);
  r[8] = k[26] + (k[27] << 8) + (k[28] << 16) + ((k[29] & 3) << 24);
  r[9] = (k[29] >> 2) + (k[30] << 6) + (k[31] << 14);
}

void toradix28_255(unsigned char out[32], unsigned int in[10]) {
  out[31] = (in[9] >> 14);
  out[30] = (in[9] >> 6) & 0xff;
  out[29] = (in[8] >> 24) + ((in[9] & 0x3f) << 2);
  out[28] = (in[8] >> 16) & 0xff;
  out[27] = (in[8] >> 8) & 0xff;
  out[26] = in[8] & 0xff;
  out[25] = (in[7] >> 18) & 0xff;
  out[24] = (in[7] >> 10) & 0xff;
  out[23] = (in[7] >> 2) & 0xff;
  out[22] = (in[6] >> 20) + ((in[7] & 0x3) << 6);
  out[21] = (in[6] >> 12) & 0xff;
  out[20] = (in[6] >> 4) & 0xff;
  out[19] = (in[5] >> 22) + ((in[6] & 0x0f) << 4);
  out[18] = (in[5] >> 14) & 0xff;
  out[17] = (in[5] >> 6) & 0xff;
  out[16] = (in[4] >> 24) + ((in[5] & 0x3f) << 2);
  out[15] = (in[4] >> 16) & 0xFF;
  out[14] = (in[4] >> 8) & 0xFF;
  out[13] = in[4] & 0xFF;
  out[12] = (in[3] >> 18) & 0xFF;
  out[11] = (in[3] >> 10) & 0xFF;
  out[10] = (in[3] >> 2) & 0xFF;
  out[9] = (in[2] >> 20) + ((in[3] & 3) << 6);
  out[8] = (in[2] >> 12) & 0xFF;
  out[7] = (in[2] >> 4) & 0xFF;
  out[6] = (in[1] >> 22) + ((in[2] & 0x0F) << 4);
  out[5] = (in[1] >> 14) & 0xFF;
  out[4] = (in[1] >> 6) & 0xFF;
  out[3] = (in[0] >> 24) + ((in[1] & 0x3f) << 2);
  out[2] = (in[0] >> 16) & 0xFF;
  out[1] = (in[0] >> 8) & 0xFF;
  out[0] = in[0] & 0xFF;
}

int crypto_scalarmult(unsigned char *q, const unsigned char *n,
                      const unsigned char *p) {
  unsigned int work226[20];
  unsigned int e226[10];
  convert_to_radix226_255(e226, n);
  e226[0] &= 0x3fffff8;
  e226[9] &= 0x1fffff;
  e226[9] |= 0x100000;

  convert_to_radix226_255(work226, p);

  mainloop226_asm(work226, e226);

  recip226(work226 + 10, work226 + 10);
  karatsuba226_255(work226, work226, work226 + 10);

  freeze(work226);
  toradix28_255(q, work226);
  return 0;
}

