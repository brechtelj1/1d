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

// lock ids
int CLOCK_ID;
int ALARM_ID;
int DISK_ID_1;
int DISK_ID_2;
int TERM_ID_1;
int TERM_ID_2;
int TERM_ID_3;
int TERM_ID_4;

// condition variables
int CLOCK_COND;
int ALARM_COND;
int DISK_COND_1;
int DISK_COND_2;
int TERM_COND_1;
int TERM_COND_2;
int TERM_COND_3;
int TERM_COND_4;

// state variables
static int ABORT_SIGNAL = 0; // signals a device to abort

// 
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
    // init locks
    P1_CreateLock("clockLock",&CLOCK_ID);
    P1_CreateLock("alarmLock",&ALARM_ID);
    P1_CreateLock("diskLock1",&DISK_ID_1);
    P1_CreateLock("diskLock2",&DISK_ID_2);
    P1_CreateLock("termLock1",&TERM_ID_1);
    P1_CreateLock("termLock2",&TERM_ID_2);
    P1_CreateLock("termLock3",&TERM_ID_3);
    P1_CreateLock("termLock4",&TERM_ID_4);
    // init condition variables
    P1_CondCreate("clockCond",CLOCK_ID,&CLOCK_COND);
    P1_CondCreate("alarmCond",ALARM_ID,&ALARM_COND);
    P1_CondCreate("diskCond1",DISK_ID_1,&DISK_COND_1);
    P1_CondCreate("diskCond2",DISK_ID_2,&DISK_COND_2);
    P1_CondCreate("termCond1",TERM_ID_1,&TERM_COND_1);
    P1_CondCreate("termCond2",TERM_ID_2,&TERM_COND_2);
    P1_CondCreate("termCond3",TERM_ID_3,&TERM_COND_3);
    P1_CondCreate("termCond4",TERM_ID_4,&TERM_COND_4);
    // set up syscall
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = SyscallHandler;
    /* create the sentinel process */
    int rc = P1_Fork("Sentinel", Sentinel, NULL, USLOSS_MIN_STACK, 6 , &pid);
    assert(rc == P1_SUCCESS);
    // should not return
    assert(0);
    return;
} /* End of startup */

// causes P1_DeviceWait to return P1_WAIT_ABORTED
// NOTE: Testing 1 signal variable, if that doesn't work
// make a signal variable for all types and their units
int P1_DeviceAbort(int type,int unit){
    int result = P1_SUCCESS;
    int lockId;
    int condId;
    // check for invalid units or types
    result = GetLockAndCond(type, unit, &lockId, &condId);
    // TODO: Set global signal for device to abort
    ABORT_SIGNAL = 1;
    // TODO: NakedSignal waiting device
    P1_NakedSignal(condId);
    return result;
}

// This function gets the lock id and condition id based on the type
// of device and the unit number for that device
int GetLockAndCond(int type, int unit, int *lockId, int *condId){
    int result = P1_SUCCESS;

    switch(type){
        case USLOSS_CLOCK_DEV:
            if(unit != 0) result = P1_INVALID_UNIT;
            lockId = CLOCK_ID;
            condId = CLOCK_COND;
            break;
        case USLOSS_ALARM_DEV:
            if(unit != 0) result = P1_INVALID_UNIT;
            lockId = ALARM_ID;
            condId = ALARM_COND;
            break;
        case USLOSS_DISK_DEV:
            if(unit == 0){ 
                lockId = DISK_ID_1; 
                condId = DISK_COND_1;
            }
            else if(unit == 1){
                lockId = DISK_ID_2;
                condId = DISK_COND_2;
            }
            else{ result = P1_INVALID_UNIT;}
            break;
        case USLOSS_TERM_DEV:
            if(unit == 0){ 
                lockId = TERM_ID_1; 
                condId = TERM_COND_1;
            }
            else if(unit == 1){
                lockId = TERM_ID_2;
                condId = TERM_COND_2;
            }
            else if(unit == 2){
                lockId = TERM_ID_3;
                condId = TERM_COND_3;
            }
            else if(unit == 3){
                lockId = TERM_ID_3;
                condId = TERM_COND_3;
            }
            else{ result = P1_INVALID_UNIT;}
            break;
        default:
            result = P1_INVALID_TYPE;
    }
    return result;
}
// makes the device wait for interrupt or an abort
int P1_DeviceWait(int type, int unit, int *status) {
    int result = P1_SUCCESS;
    int aborted = 0;
    int lockId;
    int condId;
    // disable interrupts
    P1DisableInterrupts();
    // check kernel mode
    CHECKKERNEL();
    // check type and unit 
    result = GetLockAndCond(type, unit, &lockId, &condId);
    // ensure no errors have occured
    if(result != P1_SUCCESS){
        P1EnableInterrupts();
        return result; 
    }
    // aquire lock
    P1_LOCK(lockId); 
    // while interrupt has not occurred and not aborted
    // wait for interrupt or abort on type:unit
    while(1){
        // wait
        P1_Wait(condId);

        // abort
        if(ABORT_SIGNAL == 1){
            result = P1_WAIT_ABORTED;
            ABORT_SIGNAL = 0;
        }
    }
    // if not aborted
    if(result != P1_WAIT_ABORTED){
        //  set *status to device's status using USLOSS_DeviceInput
        USLOSS_DeviceInput(type,unit,status);
    }
    // release lock
    P1_Unlock(lockId);
    // restore interrupts
    P1EnableInterrupts();
    return result;
}

// handles devices, type unit, etc. calls naked signal to wake up
// the related device
// NOTE: arg is the unit number
static void DeviceHandler(int type, void *arg) {
    int wakeupClock = 5;
    int wakeupHandler = 4;
    static int clockRupt = 0;                      
    int unit = *(int *)arg;
    int result;
    int lockId;
    int condId;
    // TODO save device status

    result = GetLockAndCond(type, unit, &lockId, &condId);
    // if clock device   NOTE: completely unsure what this is
    if(type == USLOSS_CLOCK_DEV){
        clockRupt++;
        // wake up handler every 4 ticks or 80 milliseconds
        if(clockRupt % wakeupHandler == 0){
            P1Dispatch(TRUE);
        }
        // wake up device every 5 ticks or 100 milliseconds
        if(clockRupt % wakeupClock == 0){
            P1_NakedSignal(condId);
        }
    }
    // else naked signal type:unit    (Types & units listed in USLOSS.h)
    P1_NakedSignal(condId);
}

// invoked when USLOSS starts up. inits all of phase1
// calls fork, creates the first process. First process is sentinal
// sentinal ALWAYS runs
static int Sentinel (void *notused){
    int     pid;
    int     rc;
    int     status;
    int     childStatus = 1;
    /* start the P2_Startup process */
    rc = P1_Fork("P2_Startup", P2_Startup, NULL, 4 * USLOSS_MIN_STACK, 2, &pid);
    assert(rc == P1_SUCCESS);
    // while sentinel has children
    while(1){
        // call P1GetChildStatus to get children that have quit 
        // NOTE &Status empty "What is status?"
        childStatus = P1GetChildStatus(&pid,&status);
        // check if no children
        if(childStatus == P1_NO_CHILDREN){
            break;
        }
        // if no children have quit, wait
        if(childStatus == P1_NO_QUIT){
            USLOSS_WaitInt();
        }
    }
    // sentinal quits (return doesnt matter)
    USLOSS_Console("Sentinel quitting.\n");
    return 0;
} /* End of sentinel */


// similar to P1GetChildStatus
// returns processid and status of quit child
// waits for a child to quit before doing anything
int P1_Join(int *pid, int *status) {
    int result = P1_SUCCESS;
    int loopCond = 0;
    int childStatus;
    // disable interrupts
    P1DisableInterrupts();
    // check kernel mode
    CHECKKERNEL();
    // repeat until either a child quit or there are no children
    while(loopCond == 0){
        // child pid & status returned in params
        childStatus = P1GetChildStatus(pid,status);

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