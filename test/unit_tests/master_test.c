#include <test.h>
#include <csr/csr_util.h>
#include <api_untrusted.h>

//extern uintptr_t region1;
extern uintptr_t region2;
extern uintptr_t region3;

extern uintptr_t enclave_start;
extern uintptr_t enclave_end;

#define SHARED_MEM_SYNC (0x90000000)

#define STATE_0 1
#define STATE_1 2
#define STATE_2 3
#define STATE_3 4

#define EVBASE 0x0

#define riscv_perf_cntr_begin() asm volatile("csrwi 0x801, 1")
#define riscv_perf_cntr_end() asm volatile("csrwi 0x801, 0")

void test_entry(int core_id, uintptr_t fdt_addr) {
  console_init();

  if(core_id != 0) {
    while(true) { };
  }

  cache_partition_t new_partition;

  for(int i = 0; i < 64; i++) {
    if(i == 3) {
      new_partition.lgsizes[i] = 9;
    } else if( i == 5 ) {
      new_partition.lgsizes[i] = 7;
    } else if( i <  5 ) {
      new_partition.lgsizes[i] = 4; 
    } else {
      new_partition.lgsizes[i] = 0; 
    }
  }
  printm("Change LLC partitioning\n");

  api_result_t result;
  
  riscv_perf_cntr_begin();
  result = sm_region_cache_partitioning(&new_partition);
  riscv_perf_cntr_end();
  if(result != MONITOR_OK) {
    printm("sm_region_cache_partitioning FAILED with error code %d\n", result);
    test_completed();
  }

  printm("\nTest SUCCESSFUL\n\n");
  test_completed();
}
