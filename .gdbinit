set arch riscv:rv64
set riscv use_compressed_breakpoint off
layout split
layout reg
foc cmd
set trace-commands on
set logging on
target remote localhost:1234
symbol-file build/sm.elf  
