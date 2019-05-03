/* Copy the page table in the dedicated region */

.section .text.page_table
.global root_page_table
root_page_table:
  .incbin "idpt.bin"
