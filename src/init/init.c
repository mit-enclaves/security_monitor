#include <stdbool.h>
#include <sm.h>

extern void * payload_ptr;
extern uintptr_t stack_ptr;
extern uint8_t trap_vector_from_untrusted;

void sm_init(uintptr_t fdt_boot_addr) {
  sm_state_t * sm = get_sm_state_ptr();
  
  // IMPORTANT: this will be run by *all* cores
  
  // Initialize platform core state
  platform_core_init();

  uintptr_t core_id = platform_get_core_id();
  if (core_id == 0) {
    
    console_init();

    printm("Enter sm_init\n");
    // Initialize core metadata
    for ( int i=0; i<NUM_CORES; i++ ) {
      sm->cores[i].owner = OWNER_UNTRUSTED;
      sm->cores[i].thread = ENCLAVE_THREAD_NONE;
      sm->cores[i].hls_ptr = ((uintptr_t) &stack_ptr) - (i * STACK_SIZE) - HLS_SIZE; 
      unlock_core(i); // Ensure cores aren't locked
    }

    // Initialize region metadata : untrusted SW owns all regions that do not include SM code or data.
    // Initialize untrusted metadata : untrusted SW is allowed access to all regions that do not include SM code.
    for ( int i=0; i<NUM_REGIONS; i++ ) {
      bool region_doesnt_include_sm = (uint64_t)region_id_to_addr(i) > (SM_LAST_ADDRESS);
      if(region_doesnt_include_sm) {
        sm->untrusted_regions.flags[i] = true;
      }
      sm->regions[i].owner = region_doesnt_include_sm ? OWNER_UNTRUSTED : OWNER_SM;
      sm->regions[i].type = region_doesnt_include_sm ? REGION_TYPE_UNTRUSTED : REGION_TYPE_SM;
      sm->regions[i].state = REGION_STATE_OWNED;
      unlock_region(i); // Ensure cores aren't locked. the SM must be initialized in a vaccum, with only one thread running, so this is not dangerous.
    }

    // Initialize untrusted mailboxes : all are empty and unused.
    for ( int i=0; i<NUM_UNTRUSTED_MAILBOXES; i++ ) {
      sm->untrusted_mailboxes[i].state = ENCLAVE_MAILBOX_STATE_UNUSED;
    }

    // Initialize LLC partitionning datastructures
    //uint64_t *llcCtrl = (uint64_t *) LLC_CTRL_ADDR;
    uint64_t base = 0;
    for(int rid = 0; rid < NUM_REGIONS; rid++) {
      uint64_t size = 0x4;
      //*llcCtrl = (rid << LLC_CTRL_ID_OFFSET) + (LLC_CTRL_BASE_OFFSET << 4) + LLC_CTRL_SIZE_OFFSET;
      sm->llc_partitions.lgsizes[rid] = size;
      base += (1 << size);
    }
    sm->llc_sync.waiting = 0;
    sm->llc_sync.wait = false;
    sm->llc_sync.left = 0;
    sm->llc_sync.busy = false;
    platform_lock_release(&sm->llc_sync.lock);
    
    // Unlock the SM untrusted state
    unlock_untrusted_state();

    // Initialize shared platform state
    platform_init();

    // Initialize kernel
    kernel_init(fdt_boot_addr);
    printm("kernel_init done\n");

    // Resume other cores
    send_ipi_many(NULL, IPI_SOFT);
    printm("IPIs sent\n");
    
  } else {
    // All cores but core 0 sleep until shared state is initialized
    platform_wait_for_interrupt();
    printm("Core %d wakes up\n", core_id);
  }

  // Initialize all harts kernel data structures
  kernel_init_other_core(core_id);

  while(!lock_untrusted_state()) {};
  while(!lock_core(core_id)) {};
  // Initialize memory protection
  platform_initialize_memory_protection(sm, core_id);
  unlock_untrusted_state();
  unlock_core(core_id);

  // Walk the device tree and get its address
  uintptr_t fdt_os_addr = platform_get_device_tree_addr();

  // Payload must set its own stack pointer.
  platform_jump_to_untrusted( UNTRUSTED_ENTRY, 0, core_id, fdt_os_addr);
}
