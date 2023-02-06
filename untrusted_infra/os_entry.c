#include <os_util.h>
#include <csr/csr.h>
#include <api_untrusted.h>
#include <crypto_enclave_api.h>
#include <msgq.h>

//extern uintptr_t region1;
extern uintptr_t region2;
extern uintptr_t region3;

extern uintptr_t enclave_start;
extern uintptr_t enclave_end;

#define SHARED_MEM_SYNC (0x90000000)

#define EVBASE 0x20000000

#define STATE_0 1
#define STATE_1 2
#define STATE_2 3
#define STATE_3 4

// INPUTS
extern int len_a;
extern int len_elements[];
extern char *a[];

void test_entry(int core_id, uintptr_t fdt_addr) {
  volatile int *flag = (int *) SHARED_MEM_SYNC;

  if(core_id == 0) {
    //uint64_t region1_id = addr_to_region_id((uintptr_t) &region1);
    uint64_t region2_id = addr_to_region_id((uintptr_t) &region2);
    uint64_t region3_id = addr_to_region_id((uintptr_t) &region3);

    api_result_t result;

    printm("\n");

    printm("Region block\n");

    result = sm_region_block(region3_id);
    if(result != MONITOR_OK) {
      printm("sm_region_block FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Region free\n");

    result = sm_region_free(region3_id);
    if(result != MONITOR_OK) {
      printm("sm_region_free FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Region Metadata Create\n");

    result = sm_region_metadata_create(region3_id);
    if(result != MONITOR_OK) {
      printm("sm_region_metadata_create FAILED with error code %d\n\n", result);
      test_completed();
    }

    uint64_t region_metadata_start = sm_region_metadata_start();

    enclave_id_t enclave_id = ((uintptr_t) &region3) + (PAGE_SIZE * region_metadata_start);
    uint64_t num_mailboxes = 1;

    printm("Enclave Create\n");


    result = sm_enclave_create(enclave_id, EVBASE, REGION_MASK, num_mailboxes, true);
    if(result != MONITOR_OK) {
      printm("sm_enclave_create FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Region block\n");

    result = sm_region_block(region2_id);
    if(result != MONITOR_OK) {
      printm("sm_region_block FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Region free\n");

    result = sm_region_free(region2_id);
    if(result != MONITOR_OK) {
      printm("sm_region_free FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Region assign\n");

    result = sm_region_assign(region2_id, enclave_id);
    if(result != MONITOR_OK) {
      printm("sm_region_assign FAILED with error code %d\n\n", result);
      test_completed();
    }

    uintptr_t enclave_handler_address = (uintptr_t) &region2;
    uintptr_t enclave_handler_stack_pointer = enclave_handler_address + HANDLER_LEN + (STACK_SIZE * NUM_CORES);

    printm("Enclave Load Handler\n");

    result = sm_enclave_load_handler(enclave_id, enclave_handler_address);
    if(result != MONITOR_OK) {
      printm("sm_enclave_load_handler FAILED with error code %d\n\n", result);
      test_completed();
    }

    uintptr_t page_table_address = enclave_handler_stack_pointer;

    printm("Enclave Load Page Table\n");

    result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 3, NODE_ACL);
    if(result != MONITOR_OK) {
      printm("sm_enclave_load_page_table FAILED with error code %d\n\n", result);
      test_completed();
    }

    page_table_address += PAGE_SIZE;

    printm("Enclave Load Page Table\n");

    result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 2, NODE_ACL);
    if(result != MONITOR_OK) {
      printm("sm_enclave_load_page_table FAILED with error code %d\n\n", result);
      test_completed();
    }

    page_table_address += PAGE_SIZE;

    printm("Enclave Load Page Table\n");

    result = sm_enclave_load_page_table(enclave_id, page_table_address, EVBASE, 1, NODE_ACL);
    if(result != MONITOR_OK) {
      printm("sm_enclave_load_page_table FAILED with error code %d\n\n", result);
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
        printm("sm_enclave_load_page FAILED with error code %d\n\n", result);
        test_completed();
      }

      phys_addr    += PAGE_SIZE;
      os_addr      += PAGE_SIZE;
      virtual_addr += PAGE_SIZE;

    }

    //uintptr_t enclave_sp = virtual_addr;

    uint64_t size_enclave_metadata = sm_enclave_metadata_pages(num_mailboxes);

    thread_id_t thread_id = enclave_id + (size_enclave_metadata * PAGE_SIZE);
    uint64_t timer_limit = 0xeffffffffff;

    printm("Thread Load\n");

    result = sm_thread_load(enclave_id, thread_id, EVBASE, 0x0, timer_limit); // SP is set by the enclave itself
    if(result != MONITOR_OK) {
      printm("sm_thread_load FAILED with error code %d\n\n", result);
      test_completed();
    }

    printm("Enclave Init\n");

    result = sm_enclave_init(enclave_id);
    if(result != MONITOR_OK) {
      printm("sm_enclave_init FAILED with error code %d\n\n", result);
      test_completed();
    }

    // Let other thread know we are ready
    while(*flag != STATE_0);
    *flag = STATE_1;
    asm volatile("fence");

    printm("Enclave Enter\n");

    result = sm_enclave_enter(enclave_id, thread_id);

    printm("Test SUCCESSFUL\n\n");
    test_completed();
  }
  else if (core_id == 1) {
    *flag = STATE_0;
    asm volatile("fence");
    while(*flag != STATE_1);

    init_enclave_queues();
    
    // HACKS ON HACKS - Leaves spaces for the two queues
    init_heap(SHARED_MEM_REG + (2 * sizeof(queue_t)), 500 * PAGE_SIZE);

    // key_seed_t *seed = malloc(sizeof(key_seed_t));
    uint64_t key_id;
    public_key_t *pk = malloc(sizeof(public_key_t));
    signature_t *s = malloc(sizeof(signature_t)); 

    printm("Creat SK\n");
    create_signing_key_pair(NULL, &key_id);
    printm("Creat PK\n");
    get_public_signing_key(key_id, pk);
    
    msg_t *m;
    queue_t *qresp = SHARED_RESP_QUEUE;
    int ret;
    
    do {
      ret = pop(qresp, (void **) &m);
      //if((ret == 0)) { //&& (m->f == F_VERIFY)) {
        //printm("result\n"); // %d\n", m->ret);
      //}
    } while((ret != 0) || (m->f != F_GET_SIGN_PK));

    // *** BEGINING BENCHMARK ***
    //riscv_perf_cntr_begin();

    //printm("Sign\n");
    for(int i = 0; i < 256 * 12; i++) {
      if(req_queue_is_full()) { 
        do {
          ret = pop(qresp, (void **) &m);
          //if(ret == 0) {
          //  printm("RPC with f code %d has returned\n", m->f);
          //}
        } while(!resp_queue_is_empty());
      }
      sign(a[i%len_a], len_elements[i%len_a], key_id, s);
    }

    //printm("Verify SK\n");
    //verify(s, a[0], len_elements[0], pk);

    //printm("Send Enclave Exit\n");
    enclave_exit();
    
    //printm("Done sending RPC\n");

    do {
      ret = pop(qresp, (void **) &m);
      //if((ret == 0)) { //&& (m->f == F_VERIFY)) {
        //printm("result\n"); // %d\n", m->ret);
      //}
    } while((ret != 0) || (m->f != F_EXIT));
   
    //printm("Last function %d\n", m->f); 
    //riscv_perf_cntr_end();
    // *** END BENCHMARK *** 
    printm("Received enclave exit confirmation\n");
    send_exit_cmd(0);
  }
  else {
    printm("Core n %d\n\n", core_id);
    test_completed();
  }
}
