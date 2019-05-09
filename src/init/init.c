#include <sm.h>
#include <data_structures.h>

extern void resume_hart_after_init_globals(void);

__atrribute__((section(".sm.globals"))) security_monitor_globals_t sm_globals;

__attribute__((section(".sm.text.init"))) void initialize_security_monitor_globals() {
   // INIT CORES

   for(int i = 0; i < NUM_CORES; i++) {
      sm_globals.cores[i].owner = 0;
      sm_globals.cores[i].has_enclave_schedule = false;
      sm_globals.cores[i].cur_thread = 0;
      sm_globals.cores[i].lock.flag = 0;
   }

   // INIT REGIONS
   
   aquireLock(sm_globals.regions[0].lock); // TODO get the lock
   sm_globals.regions[0].type = security_monitor_region;
   sm_globals.regions[0].owner = 1;
   sm_globals.regions[0].state = dram_region_owned;
   releaseLock(sm_globals.regions[0].lock); // TODO get the lock
   
   for(int i = 1; i < NUM_REGIONS; i++) {
      aquireLock(sm_globals.regions[i].lock); // TODO get the lock
      sm_globals.regions[i].type = untrusted_region;
      sm_globals.regions[i].owner = 0;
      sm_globals.regions[i].state = dram_region_free;
      releaseLock(sm_globals.regions[i].lock); // TODO get the lock
   }

   // TODO: update signing enclave measurement

   // TODO: Send IPI to other core to wake them up

   resume_hart_after_init_globals();
}


