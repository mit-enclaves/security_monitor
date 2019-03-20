#include <data_structures.h>

__attribute__((section(".init")))

extern security_monitor_t globals;

void initialize_security_monitor_globals() {
	// INIT CORES
	// INIT REGIONS
	for(int i = 0; i < NUM_REGIONS; i++) {
		requestLock(globals.regions[i].lock); // TODO get the lock
		if(i == 0) {
			globals.regions[i].type = security_monitor_region;
			globals.regions[i].owner = {};
			globals.regions[i].state = dram_region_owned;
		}
		else {
			globals.regions[i].type = untrusted_region;
			globals.regions[i].owner = {};
			globals.regions[i].state = dram_region_free;
		}
	}
}

resume_hart_after_init_globals
