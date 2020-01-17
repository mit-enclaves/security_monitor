



OUTPUT_ARCH( "riscv" )
ENTRY( boot_entry )

MEMORY
{
  BOOT (x) : ORIGIN = 0x1000, LENGTH = 0x10000
  SM_STATE (w) : ORIGIN = 0x80001000, LENGTH = 0x3000
  SM (wx) : ORIGIN = 0x80004000, LENGTH = 0x10000
  PAYLOAD (wx) : ORIGIN = 0x80010000, LENGTH = 0x8000000
}

SECTIONS
{
  . = ORIGIN(BOOT);
  .boot : {
    PROVIDE( boot_entry = . );
    *(.boot.text.entry)
    *(.boot.*)
    *(.boot)
  } > BOOT
}


SECTIONS
{
  . = ORIGIN(SM_STATE);
  .sm.state : {
    *(.sm.state)
  } > SM_STATE
}

SECTIONS
{
  . = ORIGIN(SM);
  .sm : {
    PROVIDE( sm_entry = . );
    *(.sm)
  } > SM
}


SECTIONS
{
  . = ORIGIN(PAYLOAD);
  .text : {
    PROVIDE( payload_entry = . );
    *(.text.entry)
    *(.text.*)
    *(.text)
  } > PAYLOAD

  .data ALIGN(0x1000) : {
    *(.data.*)
    *(.data)
  } > PAYLOAD

  .bss ALIGN(0x1000) : {
    *(.bss.*)
    *(.bss)
  } > PAYLOAD

  .rodata ALIGN(0x1000) : {
    *(.rodata.*)
    *(.rodata)
  } > PAYLOAD

  .htif ALIGN(0x1000) (NOLOAD) : {
    PROVIDE( __htif_base = . );
    *(.htif.tohost)
    *(.htif.fromhost)
  } > PAYLOAD

  .region1 ALIGN(0x2000000) (NOLOAD) : {
    PROVIDE( region1 = . );
    BYTE(1)
  } > PAYLOAD

  .region2 ALIGN(0x2000000) (NOLOAD) : {
    PROVIDE( region2 = . );
    BYTE(1)
  } > PAYLOAD

  .region3 ALIGN(0x2000000) (NOLOAD) : {
    PROVIDE( region3 = . );
    BYTE(1)
  } > PAYLOAD
}
