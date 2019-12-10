OUTPUT_ARCH( "riscv" )
ENTRY( sm_entry )

MEMORY
{
  HTIF (w) :              ORIGIN = 0x80000000,  LENGTH = 0x1000
  GLOBALS (w) :           ORIGIN = 0x80001000,  LENGTH = 0x1000
  ENCLAVE_HANDLER (wx) :  ORIGIN = 0x90000000,  LENGTH = 0x1000
}

SECTIONS
{
  . = ORIGIN(HTIF);
  .htif (NOLOAD) : {
    *(.htif)
  } > HTIF
}

SECTIONS
{
  . = ORIGIN(GLOBALS);
  .sm.keys (NOLOAD) : {
    *(.sm.keys)
  } > GLOBALS

  .sm.state (NOLOAD) : {
    *(.sm.state)
  } > GLOBALS

  .sm.signing_enclave_measurement : {
    *(.sm.signing_enclave_measurement)
  } > GLOBALS
}

SECTIONS
{
  . = ORIGIN(ENCLAVE_HANDLER);
  .enclave_handler.text : {
    PROVIDE( enclave_handler_entry = . );
    KEEP( *(.text.entry) )
    *(.text.*)
    *(.text)
  } > ENCLAVE_HANDLER

  .enclave_handler.rodata : {
    *(.rodata)  /* read-only data */
    *(.rodata.*)
  } > ENCLAVE_HANDLER

  .enclave_handler.got : {
    *(.got)   /* global offset table */
    *(.got.*)
  } > ENCLAVE_HANDLER
}
