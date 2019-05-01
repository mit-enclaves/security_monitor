#ifndef HTIF_H
#define HTIF_H

#include <stdint.h>

extern volatile uint64_t tohost;
extern volatile uint64_t fromhost;

# define TOHOST_CMD(dev, cmd, payload) \
  (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

void print_char(char c);

void print_str(char* s);

#endif // HTIF_H
