
# Include makefile fragments

# High-level targets
.PHONY: clean
clean:
	rm -rf build

.PHONY: test
test:

.PHONY: qemu
qemu:
	cd qemu && git submodule update --init --recursive
	mkdir -p build/qemu
	cd build/qemu && ../../qemu/configure --target-list=riscv64-softmmu
	cd build/qemu && make

all: sm_binary
