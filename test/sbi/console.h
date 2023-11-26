#ifndef CONSOLE_H
#define CONSOLE_H

#include <sbi_kernel.h>
#include <stdint.h>
#include <stdarg.h>

void console_init(void);

void print_char(char c);
void print_str(char* s);
void print_int(uint64_t n);

void printm(const char* s, ...);
void vprintm(const char *s, va_list args);
void putstring(const char* s);

void send_exit_cmd(int c);

#endif // CONSOLE_H
