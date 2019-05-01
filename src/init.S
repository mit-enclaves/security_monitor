#include <constants.h>

# TODO: Make sure stack never grows above 4KB

# Boot event (happens after reset, measurement, etc)
# ==================================================
.option norvc # TODO: what the fuck is this?

.section .data.sm
.align 3
platform_disabled_hart_mask: 
.dword 0

.section .text.sm.init.entry
.globl _start
_start:

# ## 1). Clear the register file, except $a0 and $a1, which receive inputs from boot
li x1, 0
li x2, 0 // sp: stack pointer
li x3, 0
li x4, 0
li x5, 0
li x6, 0
li x7, 0
li x8, 0
li x9, 0
# ### save a0 and a1; arguments from previous boot loader stage:
# x10 (a0) receives a hart (core) ID
# x11 (a1) receives a pointer to the device tree data structure
li x12, 0
li x13, 0
li x14, 0
li x15, 0
li x16, 0
li x17, 0
li x18, 0
li x19, 0
li x20, 0
li x21, 0
li x22, 0
li x23, 0
li x24, 0
li x25, 0
li x26, 0
li x27, 0
li x28, 0
li x29, 0
li x30, 0
li x31, 0
csrw mscratch, zero

# ## 2). Set the trap vector to the untrusted trap handler address
la t0, trap_vector_from_untrusted
csrw mtvec, t0
csrr t1, mtvec

# ## 3). Set the stack pointer to memory reserved for the SM stack on each hart.
# Each hart gets 1 page (4 KB) per hart for machine mode stack
# set $sp to sm_stack_ptr, provided by the linker, and is defined in stack.S
la sp, stack_ptr

# sp = sp - hartid * page_size // TODO: do not overwrite the saved states
csrr a3, mhartid
slli a2, a3, SHIFT_PAGE
sub sp, sp, a2

# ## 4). Designate the first available (unmasked) hart to perform initialization.
# All other harts will sleep and wait for initialization to be finished
# $a4 = select one hart among the enabled harts given by ~(*platform_disabled_hart_mask)
   ld a4, platform_disabled_hart_mask
   addi a5, a4, 1
   not a4, a4
   and a4, a4, a5

# a4 is the one-hot vector of the lowest-numbered hart that isn't disabled
# Check to see if *this* hart is the lowest hart that isn't disabled
   srl a4, a4, a3
   andi a4, a4, 1

# call initialize_sm, defined in init.c if this is the designated hart to perform initialization
   bnez a4, initialize_security_monitor_globals

# Else this is *not* the hart to perform initialization, so prepare to wait for initialization to finish.
# Enable inter-processor (software) interrupts on this hart
   li a2, MIE_MEIE
   csrw mie, a2

   .init.wait_on_ipi_signal:
   wfi

# disabled harts never start; loop until (if ever) the hart is not disabled
   ld a4, platform_disabled_hart_mask
   srl a4, a4, a3
   andi a4, a4, 1
   bnez a4, .init.wait_on_ipi_signal

# If a software interrupt is pendng (this hart received an IPI), the SM must have been initialized.
   csrr a2, mip
   andi a2, a2, MIP_MSIP
   beqz a2, .init.wait_on_ipi_signal

# If this hart ID is out of range for this SM binary, wait forever.
# Else, go on to initialize this hart.
   li a2, NUM_CORES
   bltu a3, a2, resume_hart_after_init_globals
   j .init.wait_on_ipi_signal

   .globl resume_hart_after_init_globals
   .type resume_hart_after_init_globals, @function
   .section .text.sm.init
   resume_hart_after_init_globals:

   /* Initialize Sanctum CSRs */

   li t0, 0
   li t1, 0xFFFFFFFFFFFFFFFF

   csrw 0x7c0, t1 // Write MEVBASE
   csrw 0x7c1, t0 // Write MEVMASK

   csrw 0x7c3, t1 // Write MMRBM

   csrw 0x7c5, t1 // Write MPARBASE
   csrw 0x7c6, t0 // Write MPARMASK

   /* Switch to S-mode, jump to the protected range */
   /** Set mPP to 1 (S-mode), sIE to 1, mPIE to 0 and TVM to 1 **/
   csrr t0, mstatus
# MSTATUS_TVM:  0x00100000
# MSTATUS_MPP:  0x00001800
# MSTATUS_mPIE: 0x00000080
# MSTATUS_SIE:  0x00000002
   li t1, 0xFFFFFFFFFFEFE77D
   li t2, 0x00100802
   and t0, t0, t1
   or t0, t0, t2
   csrw mstatus, t0

   /* Set return address to main program */
   la t0, main
   csrw mepc, t0
   mret
