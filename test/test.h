#ifndef SM_TEST_H
#define SM_TEST_H

#include "htif/htif.h"

void test_entry (void)  __attribute__((noreturn));



static inline void test_success (void) __attribute__((noreturn));
static inline void test_success (void) {
  tohost = TOHOST_CMD(0, 0, 0b01);
  while (1) { continue; }
}

// TODO: add test fail helper

#endif // SM_TEST_H
