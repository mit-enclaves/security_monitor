#include <data_structures.h>
#include <sm.h>

__attribute__((section(".init")))

security_monitor_t sm_globals;

void initialize_security_monitor_globals() {
	// INIT CORES
	// INIT REGIONS
	for(int i = 0; i < NUM_REGIONS; i++) {
		requestLock(sm_globals.regions[i].lock); // TODO get the lock
		if(i == 0) {
			sm_globals.regions[i].type = security_monitor_region;
			sm_globals.regions[i].owner = {};
			sm_globals.regions[i].state = dram_region_owned;
		}
		else {
			sm_globals.regions[i].type = untrusted_region;
			sm_globals.regions[i].owner = {};
			sm_globals.regions[i].state = dram_region_free;
		}
	}
}

resume_hart_after_init_globals
