#ifndef SECURITY_MONITOR_H
#define SECURITY_MONITOR_H
#include <data_structures.h>

extern security_monitor_t sm_globals;
extern uintptr_t _enclave_trap_handler_start;
extern uintptr_t _enclave_trap_handler_end;
extern uintptr_t _security_monitor_secret_key;

#endif // SECURITY_MONITOR_H
