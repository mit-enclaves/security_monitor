# MIT Sanctum/MI6 Security Monitor (SM)

The security monitor (SM) is part of MIT's [Sanctum](https://eprint.iacr.org/2015/564) and [MI6](https://arxiv.org/abs/1812.09822) projects.
The SM implements enclave semantics over a processor system with support for protection domains.
This implementation targets a family of RISC-V (specifically, at least `RV64IMA`) implementations, but the SM is rather generic, and should be readily portable onto another processor system with hardware support protection boundaries and some achievable notion of virtual memory.

## Building the security monitor

Building the SM should be a simple matter of performing `make sm`.
This performs a series of steps:

1. Uses the [parameterization](#parameters) to customize linker scripts
2. Builds the SM enclave handler binary (the SM code included within each enclave's domain, implementing the SM API).
3. Builds a set of identity page tables for the SM. These are used to virtualize an OS's access to physical memory.
4. Builds the SM, which includes the identity page tables and .

### Parameters

The SM is parameterized via [parameters.h](platform/parameters.h).

#### SM configuration:

| Parameter                 | Default  | Description  |
| ------------------------- | --------:|:------------ |
| `STACK_SIZE`              | `0x1000` | Memory reserved for the SM's machine-mode stack, included within the SM's footprint in memory. |
| `MAILBOX_SIZE`            | `0x100`  | The size of the largest mail message - SM's primitive for explicit message passing across protection domains. |
| `NUM_UNTRUSTED_MAILBOXES` | `8`      | The number of mailboxes allocated to the untrusted software domain (the operating system and its processes). |
| `CLEAN_REGIONS_ON_FREE`   | `true`   | If `true`, the SM erases regions (units of isolated memory exclusively allocated to a protection domain) as part of the `sm_region_free` operation. This behavior differs from Sanctum's construction. If `false`, the SM follows the Sanctum paper, meaning an enclave must clean up its own regions before it performs `sm_region_free` (All of an enclave's regions are erased when the enclave is destroyed). |

#### Placement of the SM in memory:

| Parameter       | Default      | Description  |
| --------------- | ------------:|:------------ |
| `SM_STATE_ADDR` | `0x80001000` | Address in physical memory where the SM state structure resides. |
| `SM_STATE_LEN`  | `0x3000`     | Number of bytes reserved for the SM state. This will vary depending on the number of untrusted mailboxes specified in the SM's parameters. |
| `SM_ADDR`       | `0x80003000` | The base address of the SM in physical memory. This is also the SM's entry address at boot. |
| `HANDLER_LEN`   | `0x4000`     | Number of bytes reserved for the SM enclave handler. TODO: this need not be a parameter. |
| `SM_LEN`        | `0x10000`     | Number of bytes reserved for the SM. The SM uses this this to set up the machine's protection primitives to guard itself from other software. |
| `UNTRUSTED_ENTRY` | `0x82000000` | The entry point for untrusted software (the OS). |

#### Processor system configuration

The SM's parameterization also includes a handful of architectural parameters that you shouldn't mess with unless you know quite well what you're up to.

## Integrating the SM into your software stack

The SM is *not* designed to run on its own.
Indeed, the notion of "running" the SM is not defined past initialization: the SM maintains internal data structures needed to implement enclave semantics over a low-level "platform" ABI.
To this end, the SM implements an initialization routine (to be invoked before any untrusted software, early in the boot process), and a pair of M-mode event handlers (for events originating during and outside enclaved execution, respectively).

The initialization routine prepares SM data structures and initializes the platform's protection mechanism before delegating execution to untrusted software (at `UNTRUSTED_ENTRY`).

The handler receiving untrusted events may delegate some events directly to the untrusted software (this is appropriate for timer interrupts, page faults, etc.).
The enclave mode SM handlers receives *all* events, and forces an enclave exit if the event is not handled by the SM or the enclave.
The handlers also implement the [SM API](src/api.h), through which .
In short, untrusted software and enclaves call into the SM much like a system call: via function call semantics over `ecall` (passing the API method in `$a7`, and arguments via `$a0-$a6`, receiving a result in `$a0`).
It is very important to remember that all addresses given the the SM as arguments are **physical addresses**.

In the case of a machine with paged virtual memory, the SM expects to own M-mode execution, allowing untrusted software to exist in "S" and "U" modes, and implementing "U" mode enclaves.
This implementation expects to take advantage of page table walker invariants (see Sanctum, MI6) to implement isolation boundaries, and "S"-mode is barred from disabling paged virtual memory.
To remain compatible with an OS and memory-mapped devices, the SM employs `TVM` and spoofs physical memory access by installing identity page tables.

## Debugging SM code

The SM consists of two sets of symbols: the union of (shared state, (initialization code, and the untrusted events handler)) resides at (`SM_STATE_ADDR`, `SM_ADDR`) and is described by `sm.elf`.
The enclave event handler is different, as each enclave receives its own copy of this code.
The enclave event handler has no internal state (instead accessing the shared state at `SM_STATE_ADDR`, and the SM stack, as set up by the initialization routine), and consists only of instructions and constants.
All addresses used within the enclave event handler binary are *pc-relative*, with the notable exception of `SM_STATE_ADDR` (which is a constant read from [parameters.h](platform/parameters.h))
This piece of the SM is given by `sm.enclave.elf` (for the debug symbols), and its binary is statically linked into `sm.elf`.

Productive debugging of the SM requires carefully loading the relevant debug symbols. For example, for suppose a test `null_test.elf` starts an enclave at `0x82000000`.
We would instruct gdb to load the following symbols:

- the test ELF: `file null_test.elf`, at its given addresses
- the SM: `add-symbol-file sm.elf <SM_ADDR>`, specifying the address of the SM's `.text` section to be <SM_ADDR>. Omitting this should work if the SM's linker script matches the architecture.
- the SM enclave handler: `add-symbol-file sm.enclave.elf 0x82000000`

Care should be taken to remove unnecessary symbols, as multiply defined symbols can become extremely confusing.
Use `info symbol 0x82000000` and `remove-symbol-file -a 0x82000000` to clean up.
