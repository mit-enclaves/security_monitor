#include <parameters.h>

.option norvc
.section .boot.text.entry, "ax", %progbits

li ra, SM_ADDR
jr ra
nop
