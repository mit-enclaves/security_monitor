TOP_DIR=$(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

SM_BUILD_DIR=${TOP_DIR}/build
TOOLS_DIR=${TOP_DIR}/../tools

#CC=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-gcc
#OBJCOPY=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-objcopy
CC=riscv64-unknown-elf-gcc
OBJCOPY=riscv64-unknown-elf-objcopy

QEMU=${TOOLS_DIR}/qemu/bin/qemu-system-riscv64
QEMU_FLAGS= -machine sanctum -m 2G -nographic -S -gdb tcp::1234

.PHONY: all
all: $(SM_BUILD_DIR)/sm.bin

PYTHON=python
include $(TOP_DIR)/Makefrag

.PHONY: clean
clean:
	-rm -rf $(SM_BUILD_DIR)

# Print any variable for debug
print-%: ; @echo $*=$($*)
