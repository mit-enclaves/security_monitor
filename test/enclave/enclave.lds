OUTPUT_ARCH( "riscv" )
ENTRY( enclave_entry )

SECTIONS
{
  .text : {
    PROVIDE( enclave_entry = . );
    KEEP( *(.text.entry) )
    *(.text.*)
    *(.text)
  }

  .rodata ALIGN(64) : {
    *(.rodata)  /* read-only data */
    *(.rodata.*)
  }

  .data ALIGN(64)  : {
    KEEP(*(.data.stack))
    *(.data.*)
    *(.data)  /* initialized data */
  }

  .bss ALIGN(64)  : {
    *(.bss)   /* uninitialized data */
    *(.bss.*)
  }
}
