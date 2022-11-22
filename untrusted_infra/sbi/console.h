#ifndef CONSOLE_H
#define CONSOLE_H

#include <sbi_kernel.h>
#include <stdint.h>
#include <stdarg.h>

void printm(const char* s, ...);
void vprintm(const char *s, va_list args);
void putstring(const char* s);

void send_exit_cmd(int c);

#endif // CONSOLE_H
