#ifndef SM_TEST_H
#define SM_TEST_H

#include "htif/htif.h"

void test_entry (void)  __attribute__((noreturn));

static inline void test_completed (void) __attribute__((noreturn));
static inline void test_completed (void) {
  tohost = TOHOST_CMD(0, 0, 0b01);
  while (1) { continue; }
}

#endif // SM_TEST_H
