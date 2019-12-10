OUTPUT_ARCH( "riscv" )
ENTRY( payload_entry )

MEMORY
{
  HTIF (w) :      ORIGIN = 0x80000000,  LENGTH = 0x1000
  GLOBALS (w) :   ORIGIN = 0x80001000,  LENGTH = 0x1000
  SM (wx) :       ORIGIN = 0x80002000,  LENGTH = 0x8000E000
  PAYLOAD (wx) :  ORIGIN = 0x80010000,  LENGTH = 0x7FFF0000
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
  .sm : {
    *(.sm)
  } > SM
}

SECTIONS
{
  . = ORIGIN(PAYLOAD);
  .payload (NOLOAD) : {
    PROVIDE( payload_entry = . );
    *(.payload) /* Your payload goes here */
  } > PAYLOAD
}
