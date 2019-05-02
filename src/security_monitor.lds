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
    *(.sm_keys)
    PROVIDE(sm_entry = .);
    *(.sm.text.init.entry)
    *(.sm.text.init)
    *(.sm.text.untrusted_trap.entry)
    *(.sm.text.untrusted_trap)
    *(.sm.text)
    *(.sm.data)
    /* this includes *(.text.enclave_trap.entry)
       this includes *(.text.enclave_trap)
       this includes *(.idpt) */
    *(.sm.mstacks)
  }

  . = 0x82000000;
  .os : {
    *(.os.text)
    *(.os.stack)
  }

_end = .;
}

/* TODO: inspect pre-link elf to see what we're accidentally throwing away that's necessary. .got? .rodata? */
