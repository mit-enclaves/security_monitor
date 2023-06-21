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
Building intructions can be found on the [git repository](https://github.com/mit-enclaves/qemu-sanctum/tree/riscy-ooo).

## Building the Security Monitor

Building the SM should be a simple matter of performing `make sm`.
This performs a series of steps:

1. Uses the specific parameters to customize linker scripts
2. Builds the enclave mini-SM (the SM code included within each enclave's domain).
3. Builds a set of identity page tables for the SM. These are used to virtualize an OS's access to physical memory.
4. Builds the SM.

### Other Targets

`make null_boot_loader` builds a simple bootloader that will directly enter the SM.
`make master_test` builds the simple bootloader, the SM, sets up a simple enclave and test various functionality of the API including enclave creation, enclave entry, enclave exit, enclave deletion but also shared memory and secure mailboxes.

All elfs and binary files can be found in the `build/` folder. The `make disassemble-all` and `make source-all` targets will disassemble and intermix the source code for every elfs files present in the `build/` folder.

## Running the Security Monitor

To run the SM, you will need QEMU (see instructions above).
Set up the global variable `SANCTUM_QEMU` to path leading to the `qemu-system-riscv64` executable.
You can then run `make run_master_test` to run the test.

## Debugging SM code

To debug the SM, simply run `make debug_master_test`. That will launch QEMU and pause waiting for a GDB instance. You can then launch a GDB intance by running `riscv64-unknown-elf-gdb` (that you should have built with the riscv toolchain earlier).
This should launch GDB in TUI mode, connect to QEMU, load the required symbols and have the emulator paused on the first instruction of the bootloader (address 0x1000).
All GDB instructions to set up the right symbols are can be found in [.gdbinit](.gdbinit).
Follow GDB instruction provided in the error message if the use of `.gdbinit` by GDB requires special authorization.

Interesting breakpoints to set up might be `test_entry`, `enclave_entry` but also `sm_init`, `trap_vector_from_untrusted` and `trap_vector_from_enclave` to break on SM entries.

## Integrating the SM into your software stack

To write your own untrusted application or enclave that interracts with the SM, refer to the encalve API and make sure to link the API headers located in the [API folder](api/). Example of enclaves and untrusted application interating together including skeleton-code to write your own can be foud on our [organition github](https://github.com/orgs/mit-enclaves/repositories).


## Organization of the Repository

#### [API](api/)

The `api/` folder contains the [SM API](api/api.h), includig the [part exposed to untrusted code](api/api_untrusted.h) and the [part exposed to enclaves](api/api_enclave.h).

#### [Platform](platform/)

The `platform/` folder should contains most of the code specific to our hardware platform (in our case RISC-V and more specifically our fork of [Riscy-OO](https://github.com/csail-csg/riscy-OOO)).
First it countains the [`parameters.h`](platform/parameters.h) file that details many static constants constants used to build the SM including size of structures and placement in memory.
This is where you will also find code related to [memory protection](platform/platform_memory_protection.c) but also [core initialization](platform/platform_core_init.c) and code used to [clean](platform/platform_clean_core.S) and [purge](platform/platform_purge_core.S) microarchitecture structures. Note that other parts of the SM still needs to be implemented in assembly and as a result are platform dependent.

#### [Scripts](scripts/)

The `scripts/` folder contains all python scripts used when building the SM.
This includes the script to generate identity page tables.

#### [Src](src/)

The `src/` folder contains the SM source code. In particular [the initialization code](src/init/) run when booting the machine but also [the api implementation](src/api/), [the code satisfying micro-kernel-like functionnalities](src/kernel/) (console interractions and illegal instructions handling for instance) ad the handlers for [the enclave](src/handle_enclave/) and [untrusted code](src/handle_untrusted/) interractions.

#### [Test](test/)

The `test/` folder contains code used to test the SM functionnalities including simple untrusted application and enclave code.