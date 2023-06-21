# Find the Root Directory
SM_DIR:=$(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# Define compiler
PYTHON=python3
CC=riscv64-unknown-elf-gcc

OBJCOPY=riscv64-unknown-elf-objcopy
OBJDUMP=riscv64-unknown-elf-objdump

DEBUG_ENCLAVE=1

# Flags
# -mcmodel=medany is *very* important - it ensures the program addressing is PC-relative. Ensure no global variables are used. To quote from the spec, "the program and its statically defined symbols must lie within any single 2 GiB address range. Addressing for global symbols uses lui/addi instruction pairs, which emit the R_RISCV_PCREL_HI20/R_RISCV_PCREL_LO12_I sequences."
DEBUG_FLAGS := -ggdb3
CFLAGS := -march=rv64g -mcmodel=medany -mabi=lp64 -fno-common -fno-tree-loop-distribute-patterns -std=gnu11 -Wall -O3 $(DEBUG_FLAGS)
LDFLAGS := -nostartfiles -nostdlib -static

FLAGS_DEBUG_ENCLAVE :=
ifeq ($(DEBUG_ENCLAVE), 1)
FLAGS_DEBUG_ENCLAVE += -D DEBUG_ENCLAVE=1
CFLAGS += $(FLAGS_DEBUG_ENCLAVE)
endif

# QEMU
.PHONY: check_env
check_env:
ifndef SANCTUM_QEMU
	$(error SANCTUM_QEMU is undefined)
endif

QEMU_FLAGS= -smp cpus=2 -machine sanctum -m 2G -nographic
DEBUG_QEMU_FLAGS= -S -s

# Define Directories
BUILD_DIR:=$(SM_DIR)/build
PLATFORM_DIR := $(SM_DIR)/platform
API_DIR := $(SM_DIR)/api
SM_SRC_DIR := $(SM_DIR)/src
SM_TEST_DIR := $(SM_DIR)/test
ENCLAVE_SRC_DIR := $(SM_DIR)/test/enclave
QUEUE_SRC_DIR := $(SM_DIR)/untrusted_infra/msgq

# Targets
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ALL:=

include $(SM_SRC_DIR)/Makefrag
include $(SM_TEST_DIR)/Makefrag

.PHONY: all
all: $(ALL)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)

ELFS := $(shell find $(BUILD) -name '*.elf')
ELFS_PREF := $(addprefix $(BUILD)/, $(ELFS))
DISASS = $(ELFS:.elf=.disa.out)
DISASS_SOURCES = $(ELFS:.elf=.src.out)

%.disa.out : %.elf
	$(OBJDUMP) -D $^ > $@

%.src.out : %.elf
	$(OBJDUMP) -S $^ > $@

.PHONY: disassemble-all
disassemble-all:$(DISASS)

.PHONY: source-all
source-all:$(DISASS_SOURCES)

# Print any variable for debug
print-%: ; @echo $*=$($*)
