#include <sm.h>
#include "../handles_enclave.h"

void enclave_software_interrupt_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc) {
    handle_llc_changes();
}
