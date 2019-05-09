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
    _security_monitor_secret_key = .;
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
    . = ALIGN(4);
    _sm_globals = .;
    *(.sm.globals)
    *(.sm.data)
    . = ALIGN(0x1000);
    _enclave_trap_handler_start = .;
    *(.text.enclave_trap.entry)
    *(.text.enclave_trap)
    _enclave_trap_handler_end = .;
    . = ALIGN(0x1000);
    *(.sm.mstacks)
  }

  . = 0x82000000;
  .os : {
    *(.os.text)
    *(.os.stack)
    *(.text)
    *(.rodata)
    *(.got)
    *(.got.plt)
   }

  . = 0x86000000;
  .illegal : {
    *(.illegal.data)
  }

  . = 0x8C000000;
  .enclave_binary : {
    *(.text.enclave_code.entry)
    *(.text.enclave_code)
  }

  . = 0xFFFFD000;
  .text.page_table ALIGN(0x1000) : {
    *(.text.page_table) /* Has to be in a text section to be readable */
  }

_end = .;
}

/* TODO: inspect pre-link elf to see what we're accidentally throwing away that's necessary. .got? .rodata? */
