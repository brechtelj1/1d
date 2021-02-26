/*
Phase 1b
*/

#include "phase1Int.h"
#include "usloss.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct PCB {
    int             cid;                // context's ID
    int             cpuTime;            // process's running time
    char            name[P1_MAXNAME];   // process's name
    int             priority;           // process's priority
    P1_State        state;              // state of the PCB
    // more fields here
} PCB;

static PCB processTable[P1_MAXPROC];   // the process table

void P1ProcInit(void)
{
    P1ContextInit();
    for (int i = 0; i < P1_MAXPROC; i++) {
        processTable[i].state = P1_STATE_FREE;
        // initialize the rest of the PCB
    }
    // initialize everything else

}

int P1_GetPid(void) 
{
    return 0;
}

int P1_Fork(char *name, int (*func)(void*), void *arg, int stacksize, int priority, int *pid ) 
{
    int result = P1_SUCCESS;

    // check for kernel mode
    // disable interrupts
    // check all parameters
    // create a context using P1ContextCreate
    // allocate and initialize PCB
    // if this is the first process or this process's priority is higher than the 
    //    currently running process call P1Dispatch(FALSE)
    // re-enable interrupts if they were previously enabled
    return result;
}

void 
P1_Quit(int status) 
{
    // check for kernel mode
    // disable interrupts
    // remove from ready queue, set status to P1_STATE_QUIT
    // if first process verify it doesn't have children, otherwise give children to first process
    // add ourself to list of our parent's children that have quit
    // if parent is in state P1_STATE_JOINING set its state to P1_STATE_READY
    P1Dispatch(FALSE);
    // should never get here
    assert(0);
}


int 
P1GetChildStatus(int *cpid, int *status) 
{
    int result = P1_SUCCESS;
    // do stuff here
    return result;
}

int
P1SetState(int pid, P1_State state, int lid, int vid) 
{
    int result = P1_SUCCESS;
    // do stuff here
    return result;
}

void
P1Dispatch(int rotate)
{
    // select the highest-priority runnable process
    // call P1ContextSwitch to switch to that process
}

int
P1_GetProcInfo(int pid, P1_ProcInfo *info)
{
    int         result = P1_SUCCESS;
    // fill in info here
    return result;
}







