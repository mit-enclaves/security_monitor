#ifndef CLIB_H
#define CLIB_H

#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

int memcmp(const void* s1, const void* s2,size_t n);
int memncmp(const void* s1, const void* s2,size_t n);
void* memcpy(void* dest, const void* src, size_t len);
void* memcpy_m2u(void* u_dest, const void* m_src, size_t len);
void* memcpy_u2m(void* m_dest, const void* u_src, size_t len);
void* memset(void* dest, int byte, size_t len);
int vsnprintf(char* out, size_t n, const char* s, va_list vl);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char *s);
char* strcpy(char* destination, const char *source);

#endif // CLIB_H
