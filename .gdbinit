set arch riscv:rv64
set riscv use_compressed_breakpoint off
layout split
layout reg
foc cmd
set trace-commands on
set logging on
target remote localhost:1234
add-symbol-file build/sm.elf 0x80000000
add-symbol-file build/null_test.elf 0x80020000
