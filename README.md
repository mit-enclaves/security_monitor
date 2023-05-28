# MIT Citadel Security Monitor (SM)

The security monitor (SM) is part of MIT's Citadel: the first side-channel-resistant enclaves with secure shared memory on a speculative out-of-order processor.
The SM is a small (~9K LOC), trusted piece of software running at a higher privilege mode than the hypervisor or the OS.
Its role is to link low-level invariants exposed by the hardware (e.g., if a specific memory access is authorized), and the high-level security policies defined by the platform (e.g., which enclave is currently executing on which core and which memory region does it own).
This implementation targets a family of RISC-V (specifically, at least `RV64IMA`) implementations, and makes use of special hardware features described in Citadel and implemented on the open-source Citadel processor based on [Riscy-OO](https://github.com/csail-csg/riscy-OOO).
Nevertheless, its implementation is generic and shoudl be easily portable to other architectures (mostly by editing code located in the [platform](platform) folder).
Citadel is the latest of MIT's familly of secure architectures that support secure enclaves and build on previous work such as [MI6](https://arxiv.org/abs/1812.09822) and [Sanctum](https://eprint.iacr.org/2015/564).

## Dependencies

### RISC-V Toolchain

To build the SM, you will first need a functioning riscv toolchain.
Building the toolchain yourself will allow you to target the exact architecture rv64imafd (for instance, to avoid compiling with compressed instruction) and will ensure the generated binaries can run on the Riscy-OO processor.
The toolchain can be found on the [official repo](https://github.com/riscv-collab/riscv-gnu-toolchain).
Clone the repository with the submodules and install the dependencies (see the repository's `README` for more detailed instructions).
Run the following instruction to target the right architecture

  $ ./configure --prefix=/opt/riscv_linux/ --with-arch=rv64imafd

And then build both the newlib and linux target by running (with optional -j flag to build on multiprocessor machines)

  $ make
  $ make linux

For easier debugging, add `GDB_TARGET_FLAGS_EXTRA := --enable-tui` to the `Makefile.in` file.

### QEMU

To debug the SM, tou might want to install QEMU with the special target that emulates the Riscy-OO processor. 
Instructions to build can be found on the [git repository](https://github.com/mit-enclaves/qemu-sanctum/tree/riscy-ooo).

## Building the Security Monitor

Building the SM should be a simple matter of performing `make sm`.
This performs a series of steps:

1. Uses the specific parameters to customize linker scripts
2. Builds the SM enclave handler binary (the SM code included within each enclave's domain, implementing the SM API).
3. Builds a set of identity page tables for the SM. These are used to virtualize an OS's access to physical memory.
4. Builds the SM.

### Other Targets

`make null_boot_loader` builds a simple bootloader that will directly enter the SM.
`make master_test` builds the simple bootloader, the SM, sets up a simple enclave and test various functionality of the API including shared memory and secure mailboxes.

All elfs and binary files can be found in the `build/` folder. The `make disassemble-all` and `make source-all` targets will disassemble and intermix the source code for every elfs files present in the `build/` folder.

## Running the Security Monitor

To run the SM, you will need QEMU and to set up the global variable `SANCTUM_QEMU` to the custom `qemu-system-riscv64` executable.
You can then run `make run_master_test` to run the test.

## Debugging SM code

To debug the SM, simply run `make debug_master_test`. That will launch QEMU and pause waiting for a GDB instance. You can then launch a GDB intance by running `riscv64-unknown-elf-gdb` (that you should have built with the riscv toolchain earlier).
This should launch GDB in TUI mode, connect to QEMU, load the required symbols and have the emulator paused on the first instruction of the bootloader (address 0x1000).
All GDB instructions to set up the right symbols are can be found in [.gdbinit](.gdbinit).
Follow GDB instruction provided in the error message if the use of `.gdbinit` by GDB requires special authorization.

Interesting breakpoints might be `test_entry`, `enclave_entry` but also `sm_init`, `trap_vector_from_untrusted` and `trap_vector_from_enclave` to break on SM entries. 
`

### Integrating the SM into your software stack

### QEMU


## Organization of the Repository
#### Placement of the SM in memory:

| Parameter       | Default      | Description  |
| --------------- | ------------:|:------------ |
| `SM_STATE_ADDR` | `0x80001000` | Address in physical memory where the SM state structure resides. |
| `SM_STATE_LEN`  | `0x3000`     | Number of bytes reserved for the SM state. This will vary depending on the number of untrusted mailboxes specified in the SM's parameters. |
| `SM_ADDR`       | `0x80003000` | The base address of the SM in physical memory. This is also the SM's entry address at boot. |
| `HANDLER_LEN`   | `0x4000`     | Number of bytes reserved for the SM enclave handler. TODO: this need not be a parameter. |
| `SM_LEN`        | `0x10000`     | Number of bytes reserved for the SM. The SM uses this this to set up the machine's protection primitives to guard itself from other software. |
| `UNTRUSTED_ENTRY` | `0x82000000` | The entry point for untrusted software (the OS). |
