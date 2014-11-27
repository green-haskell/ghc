/* -----------------------------------------------------------------------------
 *
 * (c) The GHC Team, 1998-1999
 *
 * Profiling interval timer
 *
 * ---------------------------------------------------------------------------*/

#include "PosixSource.h"
#include "Rts.h"

#include "Profiling.h"
#include "Proftimer.h"
#include "Capability.h"

#include "rapl-read2.h"

#ifdef PROFILING
static rtsBool do_prof_ticks = rtsFalse;       // enable profiling ticks
#endif

static rtsBool do_heap_prof_ticks = rtsFalse;  // enable heap profiling ticks

// Number of ticks until next heap census
static int ticks_to_heap_profile;

// Time for a heap profile on the next context switch
rtsBool performHeapProfile;

void
stopProfTimer( void )
{
#ifdef PROFILING
    do_prof_ticks = rtsFalse;
#endif
}

void
startProfTimer( void )
{
#ifdef PROFILING
    do_prof_ticks = rtsTrue;
#endif
}

void
stopHeapProfTimer( void )
{
    do_heap_prof_ticks = rtsFalse;
}

void
startHeapProfTimer( void )
{
    if (RtsFlags.ProfFlags.doHeapProfile &&
        RtsFlags.ProfFlags.heapProfileIntervalTicks > 0) {
        do_heap_prof_ticks = rtsTrue;
    }
}

void
initProfTimer( void )
{
    performHeapProfile = rtsFalse;

    ticks_to_heap_profile = RtsFlags.ProfFlags.heapProfileIntervalTicks;

    startHeapProfTimer();

    init_rapl_read();
}

nat total_ticks = 0;

void
handleProfTick(void)
{
#ifdef PROFILING
    total_ticks++;
    if (do_prof_ticks) {
        nat n;
        for (n=0; n < n_capabilities; n++) {
            capabilities[n]->r.rCCCS->time_ticks++;
            printf("Package energy: %.6fJ\n", get_package_energy());
        }
    }
#endif

    if (do_heap_prof_ticks) {
        ticks_to_heap_profile--;
        if (ticks_to_heap_profile <= 0) {
            ticks_to_heap_profile = RtsFlags.ProfFlags.heapProfileIntervalTicks;
            performHeapProfile = rtsTrue;
        }
    }
}
