#include <test.h>
#include <csr/csr.h>
#include <api_untrusted.h>

//extern uintptr_t region1;
extern uintptr_t region2;
extern uintptr_t region3;

extern uintptr_t enclave_start;
extern uintptr_t enclave_end;

void test_entry(int core_id, uintptr_t fdt_addr) {

  //uint64_t region1_id = addr_to_region_id((uintptr_t) &region1);
  uint64_t region2_id = addr_to_region_id((uintptr_t) &region2);
  uint64_t region3_id = addr_to_region_id((uintptr_t) &region3);

  api_result_t result;

  print_str("\n");

  print_str("Region block\n");

  result = sm_region_block(region3_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_block FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Region free\n");

  result = sm_region_free(region3_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_free FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Region Metadata Create\n");

  result = sm_region_metadata_create(region3_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_metadata_create FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uint64_t region_metadata_start = sm_region_metadata_start();

  enclave_id_t enclave_id = ((uintptr_t) &region3) + (PAGE_SIZE * region_metadata_start);
  uint64_t num_mailboxes = 1;

  print_str("Encalve Create\n");

  result = sm_enclave_create(enclave_id, 0x0, REGION_MASK, num_mailboxes, true);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_create FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Region block\n");

  result = sm_region_block(region2_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_block FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Region free\n");

  result = sm_region_free(region2_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_free FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Region assign\n");

  result = sm_region_assign(region2_id, enclave_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_assign FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uintptr_t enclave_handler_address = (uintptr_t) &region2;
  uintptr_t enclave_handler_stack_pointer = enclave_handler_address + HANDLER_LEN + (STACK_SIZE * NUM_CORES);

  print_str("Enclave Load Handler\n");

  result = sm_enclave_load_handler(enclave_id, enclave_handler_address);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_handler FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uintptr_t page_table_address = enclave_handler_stack_pointer;

  print_str("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, 0, 3, NODE_ACL);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_page_table FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  page_table_address += PAGE_SIZE;

  print_str("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, 0, 2, NODE_ACL);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_page_table FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  page_table_address += PAGE_SIZE;

  print_str("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, 0, 1, NODE_ACL);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_page_table FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uintptr_t phys_addr = page_table_address + PAGE_SIZE;
  uintptr_t os_addr = (uintptr_t) &enclave_start;
  uintptr_t virtual_addr = 0;

  uint64_t size = ((uint64_t) &enclave_end) - ((uint64_t) &enclave_start);
  int num_pages_enclave = size / PAGE_SIZE;

  if((size % PAGE_SIZE) != 0) num_pages_enclave++;

  for(int i = 0; i < num_pages_enclave; i++) {

    print_str("Enclave Load Page\n");

    result = sm_enclave_load_page(enclave_id, phys_addr, virtual_addr, os_addr, LEAF_ACL);
    if(result != MONITOR_OK) {
      print_str("sm_enclave_load_page FAILED with error code ");
      print_int(result);
      print_str("\n\n");
      test_completed();
    }

    phys_addr    += PAGE_SIZE;
    os_addr      += PAGE_SIZE;
    virtual_addr += PAGE_SIZE;

  }

  uintptr_t enclave_sp = virtual_addr;

  uint64_t size_enclave_metadata = sm_enclave_metadata_pages(num_mailboxes);

  thread_id_t thread_id = enclave_id + (size_enclave_metadata * PAGE_SIZE);
  uint64_t timer_limit = 10000;

  print_str("Thread Load\n");

  result = sm_thread_load(enclave_id, thread_id, 0x0, enclave_sp, timer_limit);
  if(result != MONITOR_OK) {
    print_str("sm_thread_load FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Enclave Init\n");

  result = sm_enclave_init(enclave_id);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_init FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  print_str("Enclave Enter\n");

  result = sm_enclave_enter(enclave_id, thread_id);

  
  print_str("Test SUCCESSFUL\n\n");
  test_completed();
}
