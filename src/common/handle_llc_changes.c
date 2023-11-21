#include <sm.h>

int handle_llc_changes() {
    volatile sm_state_t *sm = get_sm_state_ptr();
    
    // Wait wqand get the lock
    while(!platform_lock_acquire(&sm->llc_sync.lock));

    // Check if we have a pending LLC change
    if(sm->llc_sync.has_started == false) {
        return 1;
    }

    // If we do, inform everyone else we are here
    sm->llc_sync.waiting++;

    // Release the lock
    platform_lock_release(&sm->llc_sync.lock);

    // If yes spin wait and wait for everything to be done
    bool done;
    do {
        while(!platform_lock_acquire(&sm->llc_sync.lock));
        done = sm->llc_sync.done;
        platform_lock_release(&sm->llc_sync.lock); 
    } while(!done);

    // Once we are free return
    return 0;
}