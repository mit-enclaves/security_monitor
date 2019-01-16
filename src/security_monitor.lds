OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  . = ALIGN(6); # TODO: how aligned should this be? 6 alignes this to a cache line of 64B

  /* Security monitor */
  .sm : {
    *(.keys)
    PROVIDE(sm_entry = .);
    *(.text.init.entry)
    *(.text.init)
    *(.text.untrusted_trap.entry)
    *(.text.untrusted_trap)
    *(.data)
    # this includes *(.text.enclave_trap.entry)
    # this includes *(.text.enclave_trap)
    # this includes *(.idpt)
    *(.stack)
  }
  _end = .;
}

# TODO: inspect pre-link elf to see what we're accidentally throwing away that's necessary. .got? .rodata?
