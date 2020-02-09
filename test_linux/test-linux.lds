
OUTPUT_ARCH( "riscv" )
ENTRY( boot_entry )

MEMORY
{
  BOOT (ax) : ORIGIN = (0x1000), LENGTH = (0x10)
  SM_STATE (aw) : ORIGIN = 0x80000000, LENGTH = 0x3000
  SM (awx) : ORIGIN = 0x80003000, LENGTH = 0x10000
  PAYLOAD (awx) : ORIGIN = 0x82000000, LENGTH = 0x80000000
}

SECTIONS
{
  . = ORIGIN(BOOT);
  .boot : {
    PROVIDE( boot_entry = . );
    *(.boot.text.entry)
    *(.boot.*)
    *(.boot)
  } > BOOT =0x0
}


SECTIONS
{
  . = ORIGIN(SM_STATE);
  .sm.state : {
    *(.sm.state)
  } > SM_STATE =0x0
}

SECTIONS
{
  . = ORIGIN(SM);
  .sm : {
    PROVIDE( sm_entry = . );
    *(.sm)
  } > SM =0x0
}


SECTIONS
{
  . = ORIGIN(PAYLOAD);
  .linux :{
    *(.linux)
  } > PAYLOAD =0x0

  .htif ALIGN(0x1000) : {
    PROVIDE( __htif_base = . );
    *(.htif.tohost)
    *(.htif.fromhost)
  } > PAYLOAD

  . = (0x8D000000);
  .idpt (0x8D000000) : {
    *(.idpt)
  } > PAYLOAD =0x0
}
