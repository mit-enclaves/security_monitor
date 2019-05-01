OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  . = 0x80000000;
  .htif ALIGN(0x1000) (NOLOAD) : {
    *(.htif.fromhost)
    *(.htif.tohost)
  }

  . = 0x80001000;
  .boot_keys ALIGN(0x1000) (NOLOAD) : {
    /* nothing here for this specific application */
  }

  . = 0x80002000;
  /* Security monitor */
  .sm : {
    *(.keys)
    PROVIDE(sm_entry = .);
    *(.text.init.entry)
    *(.text.init)
    *(.text.untrusted_trap.entry)
    *(.text.untrusted_trap)
    *(.data)
    /* this includes *(.text.enclave_trap.entry)
       this includes *(.text.enclave_trap)
       this includes *(.idpt) */
    *(.stack)
  }
  _end = .;
}

/* TODO: inspect pre-link elf to see what we're accidentally throwing away that's necessary. .got? .rodata? */
