



OUTPUT_ARCH( "riscv" )
ENTRY( boot_entry )

MEMORY
{
  BOOT (ax) : ORIGIN = 0x1000, LENGTH = 0x10000
  SM_STATE (aw) : ORIGIN = 0x80000000, LENGTH = 0x3000
  SM (awx) : ORIGIN = 0x80003000, LENGTH = 0xF000
  PAYLOAD (awx) : ORIGIN = 0x80020000, LENGTH = 0x8000000
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
  .text : {
    PROVIDE( payload_entry = . );
    *(.text.entry)
    *(.text.*)
    *(.text)
  } > PAYLOAD =0x0

  .data ALIGN(0x1000) : {
    *(.data.*)
    *(.data)
  } > PAYLOAD =0x0

  .bss ALIGN(0x1000) : {
    *(.bss.*)
    *(.bss)
  } > PAYLOAD

  .rodata ALIGN(0x1000) : {
    *(.rodata.*)
    *(.rodata)
  } > PAYLOAD =0x0

  .htif ALIGN(0x1000) : {
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
