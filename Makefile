TOP_DIR:=$(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

BUILD_DIR:=$(TOP_DIR)/build
TOOLS_DIR:=$(TOP_DIR)/../build/

#CC=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-gcc
#OBJCOPY=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-objcopy
CC=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-gcc
OBJCOPY=${TOOLS_DIR}/riscv-gnu-toolchain/bin/riscv64-unknown-elf-objcopy

CC=riscv64-unknown-elf-gcc
OBJCOPY=riscv64-unknown-elf-objcopy

LINUX_OBJCOPY=riscv64-linux-gnu-objcopy

QEMU=${TOOLS_DIR}/qemu/riscv64-softmmu/qemu-system-riscv64
QEMU_FLAGS= -machine sanctum -m 2G -nographic
DEBUG_QEMU_FLAGS= -S -s

.PHONY: all
all: $(BUILD_DIR)/sm.bin $(BUILD_DIR)/sm.elf $(BUILD_DIR)/sm.enclave.elf

PYTHON=python
include $(TOP_DIR)/Makefrag

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

# Print any variable for debug
print-%: ; @echo $*=$($*)
