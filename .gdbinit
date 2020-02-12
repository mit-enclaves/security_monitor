set arch riscv:rv64
#set riscv use_compressed_breakpoint off
layout split
foc cmd
set trace-commands on
set logging on
target remote localhost:1234
symbol-file build/sm.elf
#symbol-file ../../build-riscv-linux/riscv-linux/vmlinux
#add-symbol-file build/sm.elf 0x80000000
#add-symbol-file build/master_test.elf 0x82000000
#add-symbol-file build/sm.enclave.elf  0x86000000
#add-symbol-file build/enclave.elf     0x0
#add-symbol-file ../../build-riscv-linux/riscv-linux/vmlinux -s .init.text 0x82000000 -s .init.data 0x82004200
add-symbol-file ../../build-riscv-linux/riscv-linux/vmlinux
