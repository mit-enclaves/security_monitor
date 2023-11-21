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

  .rodata ALIGN(8) : {
    *(.rodata)  /* read-only data */
    *(.rodata.*)
  }

  .bss ALIGN(8)  : {
    *(.bss)   /* uninitialized data */
    *(.bss.*)
  }
  
  .data ALIGN(256)  : {
    KEEP(*(.data.stack))
    *(.data.*)
    *(.data)  /* initialized data */
  }
}
