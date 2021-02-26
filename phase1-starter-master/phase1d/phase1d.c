#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <usloss.h>
#include <phase1.h>
#include <phase1Int.h>

#define CHECKKERNEL() \
    if ((USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE) == 0) USLOSS_IllegalInstruction()

static void DeviceHandler(int type, void *arg);
static void SyscallHandler(int type, void *arg);
static void IllegalInstructionHandler(int type, void *arg);

static int Sentinel(void *arg);

int RUPT_COND = 0;                   // condition variable

void startup(int argc, char **argv){
    int pid;
    P1ProcInit();
    P1LockInit();
    P1CondInit();

    // initialize device data structures
    USLOSS_IntVec[USLOSS_CLOCK_INT] = DeviceHandler;
    // put DeviceHandler into interrupt vector for the remainingdevices
    
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = SyscallHandler;

    /* create the sentinel process */
    int rc = P1_Fork("Sentinel", Sentinel, NULL, USLOSS_MIN_STACK, 6 , &pid);
    assert(rc == P1_SUCCESS);
    // should not return
    assert(0);
    return;

} /* End of startup */


// makes the device wait for interrupt or an abort
int P1_DeviceWait(int type, int unit, int *status) {
    int result = P1_SUCCESS;
    int aborted = 0;
    // disable interrupts
    P1DisableInterrupts();

    // check kernel mode
    CHECKKERNEL();

    // acquire device's lock 
    P1_LOCK(type);      // NOTE: Fairly sure passing in type is correct

    // while interrupt has not occurred and not aborted
    // wait for interrupt or abort on type:unit
    while(RUPT_COND != 1){
        // wait

        // abort

    }
    // if not aborted
    if(aborted == 0){
        //  set *status to device's status
        // NOTE: Interrupt handler should call USLOSS_DeviceInput to get status
        
        *status = type
    }
    // release lock
    P1_Unlock(type);
    // restore interrupts
    P1EnableInterrupts();
    return result;
}


static void DeviceHandler(int type, void *arg) {
    // record that interrupt occurred
    // save device status
    // if clock device
    //      naked signal clock every 5 interrupts
    //      P1Dispatch(TRUE) every 4 interrupts
    // else
    //      naked signal type:unit
}

static int Sentinel (void *notused){
    int     pid;
    int     rc;

    /* start the P2_Startup process */
    rc = P1_Fork("P2_Startup", P2_Startup, NULL, 4 * USLOSS_MIN_STACK, 2, &pid);
    assert(rc == P1_SUCCESS);

    // while sentinel has children
    //      call P1GetChildStatus to get children that have quit 
    //      wait for an interrupt via USLOSS_WaitInt
    USLOSS_Console("Sentinel quitting.\n");
    return 0;
} /* End of sentinel */

int P1_Join(int *pid, int *status) {
    int result = P1_SUCCESS;
    // disable interrupts
    P1DisableInterrupts();

    // check kernel mode
    CHECKKERNEL();

    // repeat
    //     call P1GetChildStatus  
    //     if there are children but no children have quit
    //        P1SetState(P1_STATE_JOINING)
    //        P1Dispatch(FALSE)
    // until either a child quit or there are no children
    // restore interrupts
    P1EnableInterrupts();
    return result;
}

static void
SyscallHandler(int type, void *arg) {
    USLOSS_Sysargs *sysargs = (USLOSS_Sysargs *) arg;
    USLOSS_Console("System call %d not implemented.\n", sysargs->number);
    P1_Quit(1025);
}