#include <api_untrusted.h>
#include <stdio.h>

int main(void) {
  for(int i =0; i < 64; i++) {
    int result = sm_region_owner(i);
    printf("Owner of region %d is %d\n", i, result);
  }
  return 0;
}
