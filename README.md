# security_monitor

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

The SM is parameterized via [parameters.h](src/parameters.h).

#### SM configuration:

| Parameter                 | Default  | Description  |
| ------------------------- | --------:| ------------:|
| `STACK_SIZE`              | `0x1000` | Memory reserved for the SM's machine-mode stack, included within the SM's footprint in memory. |
| `MAILBOX_SIZE`            | `0x100`  | The size of the largest mail message - SM's primitive for explicit message passing across protection domains. |
| `NUM_UNTRUSTED_MAILBOXES` | `8`      | The number of mailboxes allocated to the untrusted software domain (the operating system and its processes). |
| `CLEAN_REGIONS_ON_FREE`   | `true`   | If `true`, the SM erases regions (units of isolated memory exclusively allocated to a protection domain) as part of the `sm_region_free` operation. This behavior differs from Sanctum's construction. If `false`, the SM follows the Sanctum paper, meaning an enclave must clean up its own regions before it performs `sm_region_free` (All of an enclave's regions are erased when the enclave is destroyed). |

#### Placement of the SM in memory:

| Parameter       | Default      | Description  |
| --------------- | ------------:| ------------:|
| `SM_STATE_ADDR` | `0x80001000` | Address in physical memory where the SM state structure resides. |
| `SM_STATE_LEN`  | `0x3000`     | Number of bytes reserved for the SM state. This will vary depending on the number of untrusted mailboxes specified in the SM's parameters. |
| `SM_ADDR`       | `0x80004000` | The base address of the SM in physical memory. This is also the SM's entry address at boot. |
| `HANDLER_LEN`   | `0x2000`     | Number of bytes reserved for the SM enclave handler. TODO: this need not be a parameter. |
| `SM_LEN`        | `0x5000`     | Number of bytes reserved for the SM. The SM uses this this to set up the machine's protection primitives to guard itself from other software. |
| `PAYLOAD_ENTRY` | `0x80010000` | The entry point for untrusted software (the OS). |

#### Processor system configuration

| Parameter      | Default      | Description  |
| -------------- | ------------:|:------------ |
| `RAM_BASE`     | `0x80000000` |
| `RAM_SIZE`     | `0x80000000` |
| `REGION_SHIFT` | `25`         |
| `NUM_CORES`    | `1`          |

The SM's parameterization also includes a handful of architectural parameters that you shouldn't mess with unless you know quite well what you're up to.
