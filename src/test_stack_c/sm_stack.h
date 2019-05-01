#ifndef SM_STACK_H
#define SM_STACK_H

#include <data_structures.h>

#define PAD_SIZE (((SIZE_PAGE / (64 / 8)) - NUM_REGISTERS) * 8)

typedef struct {
   uint8_t stack[PAD_SIZE] __attribute__ ((packed));
   core_states_t core_states;
} core_m_stack_t __attribute__ ((aligned (SIZE_PAGE), packed));

typedef core_m_stack_t m_stacks_t[NUM_CORES];

extern m_stacks_t m_stacks;

#endif // SM_STACK_H
