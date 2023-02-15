#include "clib.h"
#include <kernel/unprivileged_memory.h>

void* memcpy(void* dest, const void* src, size_t len)
{
  const char* s = src;
  char *d = dest;

  if ((((uintptr_t)dest | (uintptr_t)src) & (sizeof(uintptr_t)-1)) == 0) {
    while ((void*)d < (dest + len - (sizeof(uintptr_t)-1))) {
      *(uintptr_t*)d = *(const uintptr_t*)s;
      d += sizeof(uintptr_t);
      s += sizeof(uintptr_t);
    }
  }

  while (d < (char*)(dest + len))
    *d++ = *s++;

  return dest;
}

void* memcpy_m2u(void* u_dest, const void* m_src, size_t len)
{
  const char* s = m_src;
  char *d = u_dest;

  if ((((uintptr_t)u_dest | (uintptr_t)m_src) & (sizeof(uintptr_t)-1)) == 0) {
    while ((void*)d < (u_dest + len - (sizeof(uintptr_t)-1))) {
      uintptr_t tmp = *(const uintptr_t*)s;
      store_uintptr_t((uintptr_t*)d, tmp, 0x0);
      d += sizeof(uintptr_t);
      s += sizeof(uintptr_t);
    }
  }

  while (d < (char*)(u_dest + len)) {
    uint8_t tmp = *s++;
    store_uint8_t((uint8_t *) d, tmp, 0x0);
    d++;
  }

  return u_dest;
}
