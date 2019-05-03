#ifndef HTIF_H
#define HTIF_H

#include <stdint.h>
#include <data_structures.h>

extern volatile uint64_t tohost;
extern volatile uint64_t fromhost;

# define TOHOST_CMD(dev, cmd, payload) \
  (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

void print_char(char c);

void print_str(char* s);

void print_int(uint64_t n);

void print_api_r(api_result_t res);

#endif // HTIF_H
