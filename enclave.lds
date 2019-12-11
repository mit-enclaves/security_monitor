OUTPUT_ARCH( "riscv" )
ENTRY( enclave_entry )

MEMORY
{
  ENCLAVE (wx) : ORIGIN = 0, LENGTH = 0x2000000
}


SECTIONS
{
  . = ORIGIN(ENCLAVE);
  .enclave : {
    PROVIDE( enclave_entry = . );
    KEEP( *(.text.entry) )
    *(.text.*)      /* enclave code */
    *(.text)
    *(.rodata)      /* constants data */
    *(.rodata.*)
    *(.bss)         /* uninitialized data */
    *(.bss.*)
    *(.data)        /* initialized data */
    *(.data.*)
    *(.got)         /* enclave code */
    *(.got.*)
  } > ENCLAVE

  PROVIDE( __enclave_end = ORIGIN(ENCLAVE) + LENGTH(ENCLAVE) );
}
