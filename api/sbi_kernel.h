#ifndef SBI_KERNEL_H
#define SBI_KERNEL_H

#include "api_util.h"

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2
#define SBI_CLEAR_IPI 3
#define SBI_SEND_IPI 4
#define SBI_REMOTE_FENCE_I 5
#define SBI_REMOTE_SFENCE_VMA 6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN 8
#define SBI_EXIT 9

static inline uintptr_t console_putchar(uint8_t c) {
   return SM_API_CALL(SBI_CONSOLE_PUTCHAR, c, 0, 0, 0, 0, 0, 0);
}

static inline uintptr_t console_exit(int c) {
   return SM_API_CALL(SBI_EXIT, c, 0, 0, 0, 0, 0, 0);
}

/*
static uint64_t console_getchar() {
   return SM_API_CALL(SBI_CONSOLE_GETCHAR, 0, 0, 0, 0, 0, 0, 0);
}
*/


#endif // SBI_KERNEL_H
