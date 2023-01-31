#include "os_util.h"

#include <msgq.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t))) // header size

uintptr_t heap_start;
uintptr_t heap_end;
uintptr_t max_pointer;

void init_heap(uintptr_t base, uintptr_t size) {
  heap_start = base;
  heap_end = base;
  max_pointer = base + size;
}

void *find_fit(size_t size) {
  size_t *header = (size_t *) heap_start;
  while(header < (size_t *) heap_end) {
    if(!(*header & 1) && ((*header & ~1L) >= size)) {
      return header;
    }
    header = (size_t *) ((char *)header + (*header & ~1L));
  }
  return NULL;
}

void *malloc(size_t size) {
  size_t blk_size = ALIGN(size + SIZE_T_SIZE);
  size_t *header  = find_fit(blk_size);
  if(header) {
    *header = *header | 1;
  } else {
    if((heap_end + blk_size) >= max_pointer) {
      printm("Allocation failed!\n");
      return NULL;
    }
    header = (void *) heap_end;
    heap_end += blk_size;
    *header = blk_size | 1; // mark allocated bit
  }
  return (char *)header + SIZE_T_SIZE;
}

void free(void *ptr) {
  size_t *header = (size_t *) ((char*)ptr - SIZE_T_SIZE);
  *header = *header & ~1L; // unmark allocated bit
}
