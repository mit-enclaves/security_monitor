#include <platform.h>

__attribute__((section(".text.platform_wait_for_interrupt")))

void platform_wait_for_interrupt () {
  while(1){
    asm volatile("wfi");
    uint64_t mip = read_csr(mip);
    if((mip & 0x8)){
      break;
    }
  }
} 
