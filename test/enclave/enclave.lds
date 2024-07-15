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
    _bss_start = .;
    *(.bss)   /* uninitialized data */
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    _bss_end = ALIGN(8);
  }
  
  .data ALIGN(256)  : {
    KEEP(*(.data.stack))
    PROVIDE( stack_top = . );
    *(.data.*)
    *(.data)  /* initialized data */
  }
}
