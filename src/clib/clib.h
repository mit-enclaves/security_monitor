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
void* memset(void* dest, int byte, size_t len);
int vsnprintf(char* out, size_t n, const char* s, va_list vl);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char *s);

#endif // CLIB_H
