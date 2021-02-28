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

// condition variables
int RUPT_COND = 0;              // interrupt condition

// state variables
static int SHOULD_WAIT = 0;     // wait state var
static int RUPT_OCCUR = 0;      // records num interrupts 

void startup(int argc, char **argv){
    int pid;
    P1ProcInit();
    P1LockInit();
    P1CondInit();

    // initialize device data structures
    USLOSS_IntVec[USLOSS_CLOCK_INT] = DeviceHandler;
    USLOSS_IntVec[USLOSS_ALARM_INT] = DeviceHandler;
    USLOSS_IntVec[USLOSS_DISK_INT] = DeviceHandler;
    USLOSS_IntVec[USLOSS_TERM_INT] = DeviceHandler;
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

// handles devices, type unit, etc. calls naked signal to wake up
// the related device
static void DeviceHandler(int type, void *arg) {
    int wakeupClock = 5;
    int wakeupHanlder = 4;
    static int clockRupt = 0;                      
    // maybe TODO record that interrupt occurred
    RUPT_OCCUR++;

    // TODO save device status


    // if clock device   NOTE: completely unsure what this is
    if(USLOSS_CLOCK_DEV){
        // naked signal clock every 5 interrupts

        // wake up handler every 4 ticks or 80 milliseconds
        if(clockRupt == wakeupHanlder){
            P1Dispatch(TRUE);
        }
        // wake up device every 5 ticks or 100 milliseconds
        if(clockRupt == wakeupHanlder){

        }
        
    }
    // else naked signal type:unit    (Types & units listed in USLOSS.h)
    else{
        // NOTE: Conflated naming convention on type, unsure usage
        if(type == USLOSS_CLOCK_DEV){
            P1_NakedSignal(USLOSS_CLOCK_DEV:1);
        }
        if(type == USLOSS_ALARM_DEV){
            P1_NakedSignal(USLOSS_ALARM_DEV:1);
        }
        if(type == USLOSS_DISK_DEV){
            P1_NakedSignal(USLOSS_DISK_DEV:2);
        }
        if(type == USLOSS_TERM_DEV){
            P1_NakedSignal(USLOSS_TERM_DEV:4);
        }
    }
}


static int Sentinel (void *notused){
    int     pid;
    int     rc;
    int     hasChild = 1;

    /* start the P2_Startup process */
    rc = P1_Fork("P2_Startup", P2_Startup, NULL, 4 * USLOSS_MIN_STACK, 2, &pid);
    assert(rc == P1_SUCCESS);

    // while sentinel has children
    while(hasChild){
    //      call P1GetChildStatus to get children that have quit 

    //      wait for an interrupt via USLOSS_WaitInt
    }
    USLOSS_Console("Sentinel quitting.\n");
    return 0;
} /* End of sentinel */


// similar to P1GetChildStatus
// returns processid and status of quit child
// waits for a child to quit before doing anything
int P1_Join(int *pid, int *status) {
    int result = P1_SUCCESS;
    int loopCond = 0;
    int childStatus
    // disable interrupts
    P1DisableInterrupts();
    // check kernel mode
    CHECKKERNEL();
    // repeat until either a child quit or there are no children
    while(loopCond == 0){
        // child pid & status returned in params
        childStatus = P1GetChildStatus(*pid,*status);

        // check for children, if none return none
        if(childStatus == P1_NO_CHILDREN){
            result = P1_NO_CHILDREN;
            loopCond = 1;
        }
        // if there are children but no children have quit
        if(childStatus == P1_NO_QUIT){
            P1SetState(P1_STATE_JOINING);
            P1Dispatch(FALSE);
        }
        // check if a child has quit
        if(childStatus == P1_SUCCESS){
            loopCond = 1;
        }
    }
    // enable interrupts
    P1EnableInterrupts();
    return result;
}

static void SyscallHandler(int type, void *arg) {
    USLOSS_Sysargs *sysargs = (USLOSS_Sysargs *) arg;
    USLOSS_Console("System call %d not implemented.\n", sysargs->number);
    P1_Quit(1025);
}