#include <parameters.h>

.option norvc
.section .text.entry

li sp, (RAM_BASE+RAM_SIZE)  # Top of memory

la ra, test_entry
jr ra

# test_entry should not return!
.hang:
  j .hang
