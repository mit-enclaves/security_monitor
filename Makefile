# Find the Root Directory
SM_DIR:=$(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# Define compiler
PYTHON=python
CC=riscv64-unknown-elf-gcc

OBJCOPY=riscv64-unknown-elf-objcopy

# Flags
DEBUG_FLAGS := -ggdb3
CFLAGS := -march=rv64g -mcmodel=medany -mabi=lp64 -fno-common -std=gnu11 -Wall -O0 $(DEBUG_FLAGS)
LDFLAGS := -nostartfiles -nostdlib -static

# QEMU
.PHONY: check_env
check_env:
ifndef SANCTUM_QEMU
	$(error SANCTUM_QEMU is undefined)
endif

QEMU_FLAGS= -machine sanctum -m 2G -nographic
DEBUG_QEMU_FLAGS= -S -s

# Define Directories
BUILD_DIR:=$(SM_DIR)/build
PLATFORM_DIR := $(SM_DIR)/platform
API_DIR := $(SM_DIR)/api
SM_SRC_DIR := $(SM_DIR)/src
SM_TEST_DIR := $(SM_DIR)/test
ENCLAVE_SRC_DIR := $(SM_DIR)/test/enclaves

# Targets
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

include $(SM_SRC_DIR)/Makefrag
include $(SM_TEST_DIR)/Makefrag

.PHONY: all
all: $(BUILD_DIR)/sm.bin $(BUILD_DIR)/sm.elf $(BUILD_DIR)/sm.enclave.elf

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

# Print any variable for debug
print-%: ; @echo $*=$($*)
