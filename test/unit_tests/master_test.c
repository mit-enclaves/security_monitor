#include <test.h>
#include <api_untrusted.h>

//extern uintptr_t region1;
extern uintptr_t region2;
extern uintptr_t region3;

extern uintptr_t enclave_start;
extern uintptr_t enclave_end;

void test_entry(void) {
  //uint64_t region1_id = addr_to_region_id((uintptr_t) &region1);
  uint64_t region2_id = addr_to_region_id((uintptr_t) &region2);
  uint64_t region3_id = addr_to_region_id((uintptr_t) &region3);

  api_result_t result;

  print_str("\n");

  result = sm_region_block(region3_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_block FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  result = sm_region_free(region3_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_free FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

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
  uint64_t timer_limit = 10000;

  result = sm_enclave_create(enclave_id, 0x0, REGION_MASK, num_mailboxes, timer_limit, true);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_create FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  result = sm_region_block(region2_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_block FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  result = sm_region_free(region2_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_free FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  result = sm_region_assign(region2_id, enclave_id);
  if(result != MONITOR_OK) {
    print_str("sm_region_assign FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uintptr_t enclave_handler_address = (uintptr_t) &region2;
  uintptr_t enclave_handler_stack_pointer = enclave_handler_address + HANDLER_LEN + STACK_SIZE;

  result = sm_enclave_load_handler(enclave_id, enclave_handler_address);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_handler FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  uintptr_t page_table_address = enclave_handler_stack_pointer;

  result = sm_enclave_load_page_table(enclave_id, page_table_address, 0, 3, NODE_ACL);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_page_table FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  page_table_address += PAGE_SIZE;

  result = sm_enclave_load_page_table(enclave_id, page_table_address, 0, 2, NODE_ACL);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_load_page_table FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  page_table_address += PAGE_SIZE;

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

  int num_pages_enclave = (((uint64_t) &enclave_end) - ((uint64_t) &enclave_start)) / PAGE_SIZE;

  print_str("Load ");
  print_int(num_pages_enclave);
  print_str(" enclave pages\n");

  if(num_pages_enclave != 2) {
    print_str("ERROR num_pages_enclave is different form 2\n");
    test_completed();
  }
  
  aes_nonce_t nonces[2] = \
  {{{0xc7, 0x9a, 0x0a, 0x11, 0xbe, 0x12, 0x55, 0x0b, 0xac, 0xc8, 0x56, 0xad, 0xa5, 0x26, 0xec, 0x98}}, \
    {{0xfb, 0x68, 0x70, 0x80, 0xf4, 0xcc, 0xeb, 0x3b, 0xeb, 0x20, 0x5e, 0xfe, 0x65, 0xe7, 0x7a, 0xfd}}};

  for(int i = 0; i < 2; i++) {

    result = sm_enclave_load_page(enclave_id, phys_addr, virtual_addr, os_addr, LEAF_ACL, &nonces[i]);
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

  uint64_t size_enclave_metadata = sm_enclave_metadata_pages(num_mailboxes);

  thread_id_t thread_id = enclave_id + (size_enclave_metadata * PAGE_SIZE);

  result = sm_thread_load(enclave_id, thread_id, 0x0, 0x1000, enclave_handler_address, enclave_handler_stack_pointer);
  if(result != MONITOR_OK) {
    print_str("sm_thread_load FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  result = sm_enclave_init(enclave_id);
  if(result != MONITOR_OK) {
    print_str("sm_enclave_init FAILED with error code ");
    print_int(result);
    print_str("\n\n");
    test_completed();
  }

  hash_t hash = {{0xca, 0x2, 0x2c, 0x1d, 0xea, 0x81, 0xf4, 0x80, 0x24, 0xe6, 0x4a, 0xb2, 0xf5, 0x38, 0x9d, 0xd1, 0xda, 0xf0, 0x38, 0x66, 0x78, 0x58, 0x20, 0x70, 0xb2, 0xe0, 0x9f, 0x24, 0x24, 0x4b, 0x58, 0x9d, 0x1d, 0x71, 0xea, 0x9b, 0xa2, 0xe4, 0x61, 0x58, 0x6, 0xf7, 0xd1, 0x23, 0xde, 0x9e, 0x88, 0x8, 0xbf, 0x8f, 0x10, 0xd7, 0xed, 0x61, 0xf4, 0xd5, 0x19, 0xb5, 0x83, 0x35, 0xdd, 0xcc, 0x94, 0xad}};
  
  result = sm_enclave_enter(enclave_id, thread_id, &hash);
  
  print_str("sm_enclave_enter completed with potential error code ");
  print_int(result);
  print_str("\n\n");
  
  if(result != 2) {
    print_str("Test SUCCESSFUL\n\n");
  }
  
  test_completed();
}
