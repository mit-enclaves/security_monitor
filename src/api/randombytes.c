#include <sm.h>
#include <stdint.h>

volatile uint64_t *random = (volatile uint64_t *)0x51000000ul;
uint64_t sm_internal_random ( ) {
    uint64_t result = *random;
    //printm("sm_random %08x\n", random);
    return result;
}
