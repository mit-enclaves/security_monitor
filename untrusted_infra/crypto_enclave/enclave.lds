OUTPUT_ARCH( "riscv" )
ENTRY( enclave_entry )

SECTIONS
{
  . = 0x20000000;
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

  .data ALIGN(64)  : {
    KEEP(*(.data.stack))
    PROVIDE( stack_top = . );
    *(.data.*)
    *(.data)  /* initialized data */
  }

  .bss ALIGN(8)  : {
    _bss_start = .;
    *(.bss)   /* uninitialized data */
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    _bss_end = .;
  }
}
