#ifndef OS_H
#define OS_H

#include <stdio.h>
#include "sbi/console.h"
#include <parameters.h>

void os_entry(int core_id, uintptr_t fdt_addr) __attribute__((noreturn));

static inline void test_completed (void) __attribute__((noreturn));
static inline void test_completed (void) {
  print_str("END TEST\n");
  while(1);
  //send_exit_cmd(0);  
}

#define PAGE_SIZE (1<<PAGE_SHIFT)

#define REGION_SIZE (1<<REGION_SHIFT)
#define REGION_MASK (~(REGION_SIZE-1))

static inline uint64_t addr_to_region_id (uintptr_t addr) {
  return ((addr-RAM_BASE) & REGION_MASK) >> REGION_SHIFT; // will return an illegally large number in case of an address outside RAM. CAUTION!
}

static inline void * region_id_to_addr (uint64_t region_id) {
  return (void *)(RAM_BASE + (region_id << REGION_SHIFT));
}

#define SHARED_MEM_REG (0x8a000000)
#define SHARED_QUEUE ((queue_t *) SHARED_MEM_REG)

void init_heap();
void *malloc(size_t size);

#endif // OS_H
