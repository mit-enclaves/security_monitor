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

void test_entry(int core_id, uintptr_t fdt_addr) {
  volatile int *flag = (int *) SHARED_MEM_SYNC;
  console_init();

  if(core_id != 0) {
    while(true) {
      if(*flag == STATE_1) {
       api_result_t res = sm_region_update();
       if(res == MONITOR_OK) {
        *flag = STATE_2;
       }
      }
    };
  }

  *flag = STATE_0;

  //uint64_t region1_id = addr_to_region_id((uintptr_t) &region1);
  uint64_t region2_id = addr_to_region_id((uintptr_t) &region2);
  uint64_t region3_id = addr_to_region_id((uintptr_t) &region3);

  api_result_t result;

  printm("\n");

  printm("Region block\n");

  result = sm_region_block(region3_id);
  if(result != MONITOR_OK) {
    printm("sm_region_block FAILED with error code %d \n\n", result);
    test_completed();
  }

  *flag = STATE_1;

  printm("Region free\n");

  do { result = sm_region_free(region3_id); } 
  while((result == MONITOR_INVALID_STATE) || (result == MONITOR_CONCURRENT_CALL));
  if(result != MONITOR_OK) {
    printm("sm_region_free FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Region Metadata Create\n");

  result = sm_region_metadata_create(region3_id);
  if(result != MONITOR_OK) {
    printm("sm_region_metadata_create FAILED with error code %d \n\n", result);
    test_completed();
  }

  uint64_t region_metadata_start = sm_region_metadata_start();

  enclave_id_t enclave_id = ((uintptr_t) &region3) + (PAGE_SIZE * region_metadata_start);
  uint64_t num_mailboxes = 1;

  printm("Encalve Create\n");

  result = sm_enclave_create(enclave_id, EVBASE, REGION_MASK, num_mailboxes, true);
  if(result != MONITOR_OK) {
    printm("sm_enclave_create FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Region block\n");

  result = sm_region_block(region2_id);
  if(result != MONITOR_OK) {
    printm("sm_region_block FAILED with error code %d \n\n", result);
    test_completed();
  }

  *flag = STATE_1;

  printm("Region free\n");

  do { result = sm_region_free(region2_id); } 
  while((result == MONITOR_INVALID_STATE) || (result == MONITOR_CONCURRENT_CALL));
  if(result != MONITOR_OK) {
    printm("sm_region_free FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Region assign\n");

  result = sm_region_assign(region2_id, enclave_id);
  if(result != MONITOR_OK) {
    printm("sm_region_assign FAILED with error code %d \n\n", result);
    test_completed();
  }

  uintptr_t enclave_handler_address = (uintptr_t) &region2;
  uintptr_t enclave_handler_stack_pointer = enclave_handler_address + HANDLER_LEN + (STACK_SIZE * NUM_CORES);

  printm("Enclave Load Handler\n");

  result = sm_enclave_load_handler(enclave_id, enclave_handler_address);
  if(result != MONITOR_OK) {
    printm("sm_enclave_load_handler FAILED with error code %d \n\n", result);
    test_completed();
  }

  uintptr_t page_table_address = enclave_handler_stack_pointer;

  printm("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 3, NODE_ACL);
  if(result != MONITOR_OK) {
    printm("sm_enclave_load_page_table FAILED with error code %d \n\n", result);
    test_completed();
  }

  page_table_address += PAGE_SIZE;

  printm("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 2, NODE_ACL);
  if(result != MONITOR_OK) {
    printm("sm_enclave_load_page_table FAILED with error code %d \n\n", result);
    test_completed();
  }

  page_table_address += PAGE_SIZE;

  printm("Enclave Load Page Table\n");

  result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 1, NODE_ACL);
  if(result != MONITOR_OK) {
    printm("sm_enclave_load_page_table FAILED with error code %d \n\n", result);
    test_completed();
  }

  uintptr_t phys_addr = page_table_address + PAGE_SIZE;
  uintptr_t os_addr = (uintptr_t) &enclave_start;
  uintptr_t virtual_addr = EVBASE;

  uint64_t size = ((uint64_t) &enclave_end) - ((uint64_t) &enclave_start);
  int num_pages_enclave = size / PAGE_SIZE;

  if((size % PAGE_SIZE) != 0) num_pages_enclave++;

  for(int i = 0; i < num_pages_enclave; i++) {

    printm("Enclave Load Page\n");

    result = sm_enclave_load_page(enclave_id, phys_addr, virtual_addr, os_addr, LEAF_ACL);
    if(result != MONITOR_OK) {
      printm("sm_enclave_load_page FAILED with error code %d \n\n", result);
      test_completed();
    }

    phys_addr    += PAGE_SIZE;
    os_addr      += PAGE_SIZE;
    virtual_addr += PAGE_SIZE;

  }

  uintptr_t enclave_sp = virtual_addr;

  uint64_t size_enclave_metadata = sm_enclave_metadata_pages(num_mailboxes);

  thread_id_t thread_id = enclave_id + (size_enclave_metadata * PAGE_SIZE);
  uint64_t timer_limit = 0xfffffff;

  printm("Thread Load\n");

  result = sm_thread_load(enclave_id, thread_id, EVBASE, enclave_sp, timer_limit);
  if(result != MONITOR_OK) {
    printm("sm_thread_load FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Enclave Init\n");

  result = sm_enclave_init(enclave_id);
  if(result != MONITOR_OK) {
    printm("sm_enclave_init FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Enclave Enter\n\n");

  result = sm_enclave_enter(enclave_id, thread_id);

  printm("\nTest MailBoxes\n");

  char* msg = "Hello Enclave!";
  result = sm_mail_send(enclave_id, 0, msg);
  if(result != MONITOR_OK) {
    printm("sm_mail_send FAILED with error code %d \n\n", result);
    test_completed();
  }
  
  result = sm_mail_accept(0, enclave_id);
  if(result != MONITOR_OK) {
    printm("sm_mail_accept FAILED with error code %d \n\n", result);
    test_completed();
  }
  
  printm("Enclave Enter a Second Time\n\n");

  result = sm_enclave_enter(enclave_id, thread_id);
  
  char msg_in[MAILBOX_SIZE];
  hash_t exp_sndr;
  result = sm_mail_receive(0, &msg_in, &exp_sndr);
  if(result != MONITOR_OK) {
    printm("sm_mail_receive FAILED with error code %d \n\n", result);
    test_completed();
  }
  
  printm("\nSender enclave measurement : [");
  for(int i = 0; i < sizeof(hash_t); i++) {
    printm("%x, ", exp_sndr.bytes[i]);
  }
  printm("]\n");
  
  printm("The enclave has sent: \n\"%s\"\n", msg_in);

  printm("\nGet enclave's certificate\n");
  hash_t e_m;
  public_key_t e_pk;
  signature_t e_sig;
  result = sm_enclave_get_attest(enclave_id, &e_m, &e_pk, &e_sig);
  if(result != MONITOR_OK) {
    printm("sm_mail_accept FAILED with error code %d \n\n", result);
    test_completed();
  }
  
  printm("Enclave measurement : [");
  for(int i = 0; i < sizeof(hash_t); i++) {
    printm("%x, ", e_m.bytes[i]);
  }
  printm("]\n");

  printm("Enclave PK : [");
  for(int i = 0; i < sizeof(public_key_t); i++) {
    printm("%x, ", e_pk.bytes[i]);
  }
  printm("]\n");
  
  printm("Enclave Certificate : [");
  for(int i = 0; i < sizeof(signature_t); i++) {
    printm("%x, ", e_sig.bytes[i]);
  }
  printm("]\n");

  printm("Delete thread\n");
  result = sm_thread_delete(thread_id);
  if(result != MONITOR_OK) {
    printm("sm_thread_delete FAILED with error code %d\n", result);
    test_completed();
  }

  printm("Delete enclave\n");
  result = sm_enclave_delete(enclave_id);
  if(result != MONITOR_OK) {
    printm("sm_enclave_delete FAILED with error code %d\n", result);
    test_completed();
  }
  
  printm("Region free\n");

  result = sm_region_free(region2_id);
  if(result != MONITOR_OK) {
    printm("sm_region_free FAILED with error code %d \n\n", result);
    test_completed();
  }

  printm("Assign region 2\n");
  result = sm_region_assign(region2_id, OWNER_UNTRUSTED);
  if(result != MONITOR_OK) {
    printm("sm_region_assign FAILED with error code %d\n", result);
    test_completed();
  }

  printm("Flush region 2\n");
  result = sm_region_flush(region2_id);
  if(result != MONITOR_OK) {
    printm("sm_region_flush FAILED with error code %d\n", result);
    test_completed();
  }

  cache_partition_t new_partition;

  for(int i = 0; i < 64; i++) {
    if(i == 3) {
      new_partition.lgsizes[i] = 9;
    } else if( i == 1 ) {
      new_partition.lgsizes[i] = 7;
    } else if( i <  16 ) {
      new_partition.lgsizes[i] = 4; 
    } else {
      new_partition.lgsizes[i] = 0; 
    }
  }
  printm("Change LLC partitioning\n");
  result = sm_region_cache_partitioning(&new_partition);
  if(result != MONITOR_OK) {
    printm("sm_region_cache_partitioning FAILED with error code %d\n", result);
    test_completed();
  }

  printm("\nTest SUCCESSFUL\n\n");
  test_completed();
}
