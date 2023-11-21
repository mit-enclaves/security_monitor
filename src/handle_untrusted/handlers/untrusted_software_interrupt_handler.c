#include <sm.h>
#include "../handles_untrusted.h"

void untrusted_software_interrupt_handler(uintptr_t mentry_ipi) {
    if(mentry_ipi & IPI_SOFT) {
        if(handle_llc_changes() == 0) {
            return;
        }
        set_csr(mip, MIP_SSIP);
        return;
    } else if(mentry_ipi & IPI_FENCE_I) {
        asm volatile ("fence.i");
    } else if(mentry_ipi & IPI_SFENCE_VMA) {
        asm volatile ("sfence.vma");
    } else if(mentry_ipi & IPI_HALT) {
        asm volatile ("wfi");
    }
    return;
}
