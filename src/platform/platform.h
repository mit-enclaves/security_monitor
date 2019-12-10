#ifndef SM_PLATFORM_H
#define SM_PLATFORM_H

void platform_panic (uint64_t error_code) __attribute__((noreturn));

void platform_purge_core (void);

void platform_clean_core(void);

#endif // SM_PLATFORM_H
