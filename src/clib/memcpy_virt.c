#include "clib.h"
#include <kernel/unprivileged_memory.h>

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

void* memcpy_u2m(void* m_dest, const void* u_src, size_t len)
{
  const char* s = u_src;
  char *d = m_dest;

  if ((((uintptr_t)m_dest | (uintptr_t)u_src) & (sizeof(uintptr_t)-1)) == 0) {
    while ((void*)d < (m_dest + len - (sizeof(uintptr_t)-1))) {
      *(uintptr_t*)d = load_uintptr_t((const uintptr_t*)s, 0x0);
      d += sizeof(uintptr_t);
      s += sizeof(uintptr_t);
    }
  }

  while (d < (char*)(m_dest + len)) {
    *d++ = load_uint8_t((uint8_t *) s, 0x0);
    s++;
  }

  return m_dest;
}
