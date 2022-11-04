#include "os_util.h"

#include <msgq.h>

uintptr_t last_pointer = 0;

void init_heap(void) {
  last_pointer = SHARED_MEM_REG + sizeof(queue_t) + sizeof(queue_t);
}

void *malloc(size_t size) {
  void *ret = (void *) last_pointer;
  last_pointer += size;
  if(last_pointer >= (SHARED_MEM_REG + REGION_SIZE)) {
    return NULL;
  }
  return ret;
}
