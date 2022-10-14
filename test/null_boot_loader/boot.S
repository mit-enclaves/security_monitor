#include <parameters.h>

.option norvc
.section .boot.text.entry, "ax", %progbits

la a1, _device_tree # device tree follows the end of this
li ra, SM_ADDR
jr ra
