OUTPUT_ARCH( "riscv" )
ENTRY( sm_entry )

MEMORY
{
  HTIF (w) :      ORIGIN = 0x80000000,  LENGTH = 0x1000
  GLOBALS (w) :   ORIGIN = 0x80001000,  LENGTH = 0x1000
  SM (wx) :       ORIGIN = 0x80002000,  LENGTH = 0x8000E000
  PAYLOAD (wx) :  ORIGIN = 0x80200000,  LENGTH = 0x7FE00000
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
  .keys : {
    PROVIDE( sm_keys = . );
    *(.sm.keys)
  } > GLOBALS

  .global.state : {
    PROVIDE( sm_state = . );
    *(.sm.state)
  } > GLOBALS
}

SECTIONS
{
  . = ORIGIN(SM);
  .sm.text : {
    PROVIDE( sm_entry = . );
    KEEP( *(.text.entry) )
    *(.text.*)
    *(.text)
  } > SM

  .sm.rodata : {
    *(.rodata)  /* read-only data */
    *(.rodata.*)
  } > SM

  .sm.data : {
    *(.bss)   /* uninitialized data */
    *(.bss.*)
    *(.data)  /* initialized data */
    *(.data.*)
  } > SM

  .sm.enclave_handler : {
    PROVIDE( sm_enclave_handler_start = . );
    *(.sm.enclave_handler)
    PROVIDE( sm_enclave_handler_end = . );
  } > SM

  .sm.got : {
    *(.got)   /* global offset table */
    *(.got.*)
  } > SM
}

SECTIONS
{
  . = ORIGIN(PAYLOAD);
  .payload (NOLOAD) : {
    PROVIDE( payload_entry = . );
  } > PAYLOAD
}
