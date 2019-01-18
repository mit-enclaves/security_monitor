# security_monitor

## Building the security monitor

Parameters
- `NUM_CORES`: how many cores does the SM manage?
- `NUM_DRAM_REGIONS`: how many DRAM regions exist in a machine with DRAM regions?


What shall we do with platform_disabled_hart_mask?

define STACK_SIZE

## Building the RISC-V tool chain (With GDB TUI enabled)

https://github.com/riscv/riscv-gnu-toolchain

1-Clone the repository
2-Edit the Makefile: add the line `GDB_TARGET_FLAGS_EXTRA := --enable-gui`
3-Run `sudo apt install libncurses5-dev`
4-Follow the instructions in the repository's README
