#include <sm_stack.h>
#include <stdio.h>

__attribute__((section(".stacks")))

m_stacks_t m_stacks;

int main(){
   printf("size stack struct ; %ld\n", sizeof(m_stacks_t));
   
}
