#include <sm_stack.h>

__attribute__((section(".stacks")))

asm volatile (".globl stack_base, stack_ptr\n
stack_base:\n");

m_stacks_t m_stacks;

asm volatile ("stack_ptr:\n");
