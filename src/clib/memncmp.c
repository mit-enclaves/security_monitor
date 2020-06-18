#include "clib.h"

int memncmp(const void* s1, const void* s2,size_t n)
{
  const unsigned char *p1 = s1, *p2 = s2;
  int res = 0;
  while(n--) {
    res = res | ((*p1 - *p2) != 0);
    p1++,p2++;
  }
  return res;
}

