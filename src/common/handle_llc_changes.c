#include <sm.h>

int handle_llc_changes() {
    sm_state_t *sm = get_sm_state_ptr();
    
    // Wait wqand get the lock
    while(!platform_lock_acquire(&sm->llc_sync.lock));

    // Check if we have a pending LLC change
    if(sm->llc_sync.busy == false) {
        platform_lock_release(&sm->llc_sync.lock); 
        return 1;
    }

    // If we do, inform everyone else we are here
    sm->llc_sync.waiting++;

    // Release the lock
    platform_lock_release(&sm->llc_sync.lock);

    // If yes spin wait and wait for everything to be done
    bool wait;
    do {
        while(!platform_lock_acquire(&sm->llc_sync.lock));
        wait = sm->llc_sync.wait;
        if(!wait) {
            sm->llc_sync.left++;
        }
        platform_lock_release(&sm->llc_sync.lock); 
    } while(wait);

    // Once we are free return
    return 0;
}
