#include "htif.h"
#include <stdint.h>

volatile uint64_t *tohost = (uint64_t *) HTIF_BASE;
volatile uint64_t *fromhost = (uint64_t *) (HTIF_BASE + 0x8);
