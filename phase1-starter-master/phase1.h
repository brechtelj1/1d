/*
 * These are the definitions for Phase 1 of the project (the kernel).
 * Version 1.8
 * DO NOT MODIFY THIS FILE.
 */

#ifndef _PHASE1_H
#define _PHASE1_H

#include "usloss.h"
#include "usyscall.h"

/*
 * Maximum number of processes.
 */

#define P1_MAXPROC  50

/*
 * Maximum number of locks and condition variables.
 */

#define P1_MAXLOCKS   2000
#define P1_MAXCONDS P1_MAXLOCKS

/*
 * Maximum number of tags.
 */
#define P1_MAXTAG 2

/*
 * Maximum length of a process or semaphore name, including
 * trailing '\0'.
 */
#define P1_MAXNAME 80
 

/*
 * Booleans
 */
#define FALSE 0
#define TRUE 1
 
#ifndef CHECKRETURN
#define CHECKRETURN __attribute__((warn_unused_result))
#endif

/*
 * Different states of a process/PCB.
 */
typedef enum P1_State {
    P1_STATE_FREE = 0,      // PCB is not in use
    P1_STATE_RUNNING,       // process is currently running
    P1_STATE_READY,         // process is ready (runnable)
    P1_STATE_QUIT,          // process has quit
    P1_STATE_BLOCKED,       // process is blocked on a lock or condition variable
    P1_STATE_JOINING        // process is waiting for a child to quit
} P1_State;

/*
 * Structure returned by P1_GetProcInfo.
 */
typedef struct P1_ProcInfo {
    char        name[P1_MAXNAME];
    P1_State    state;                  // process's state
    int         lid;                    // lock on which process is blocked, if any
    int         vid;                    // condition on which process is blocked, if any
    int         priority;               // process's priority
    int         cpu;                    // CPU consumed (in microseconds)
    int         parent;                 // parent PID
    int         children[P1_MAXPROC];   // childen PIDs
    int         numChildren;            // # of children
} P1_ProcInfo;



/*
 * External function prototypes for this phase.
 */

// Phase1b
extern  int             P1_Fork(char *name, int(*func)(void *), void *arg,
                                int stackSize, int priority, int *pid) CHECKRETURN;
extern  void            P1_Quit(int status);
extern  int             P1_GetPid(void) CHECKRETURN;
extern  int             P1_GetProcInfo(int pid, P1_ProcInfo *info) CHECKRETURN;

extern  int             P1_Join(int *pid, int *status) CHECKRETURN;

extern  int             P1_LockCreate(char *name, int *lid) CHECKRETURN;
extern  int             P1_LockFree(int lid) CHECKRETURN;
extern  int             P1_Lock(int lid) CHECKRETURN;
extern  int             P1_Unlock(int lid) CHECKRETURN;
extern  int             P1_LockName(int lid, char *name, int len) CHECKRETURN;
extern  int             P1_CondCreate(char *name, int lid, int *vid) CHECKRETURN;
extern  int             P1_CondFree(int vid) CHECKRETURN;
extern  int             P1_Wait(int vid) CHECKRETURN;
extern  int             P1_Signal(int vid) CHECKRETURN;
extern  int             P1_Broadcast(int vid) CHECKRETURN;
extern  int             P1_NakedSignal(int vid) CHECKRETURN;
extern  int             P1_CondName(int vid, char *name, int len) CHECKRETURN;
extern  int             P1_DeviceWait(int type, int unit, int *status) CHECKRETURN;
extern int              P1_DeviceAbort(int type, int unit) CHECKRETURN;

extern  int             P2_Startup(void *arg) CHECKRETURN;

/*
 * Error codes
 */

#define P1_SUCCESS 0
#define P1_TOO_MANY_PROCESSES -1
#define P1_TOO_MANY_CONTEXTS P1_TOO_MANY_PROCESSES
#define P1_INVALID_STACK -2
#define P1_INVALID_PRIORITY -3
#define P1_INVALID_TAG -4
#define P1_NO_CHILDREN -5
#define P1_NO_QUIT -6
#define P1_TOO_MANY_LOCKS -7
#define P1_TOO_MANY_CONDS -8
#define P1_NAME_IS_NULL -9
#define P1_DUPLICATE_NAME -10
#define P1_INVALID_LOCK -11
#define P1_INVALID_COND -12
#define P1_LOCK_NOT_HELD -13
#define P1_BLOCKED_PROCESSES -14
#define P1_INVALID_PID -15
#define P1_INVALID_CID -16
#define P1_INVALID_STATE -17
#define P1_INVALID_TYPE -18
#define P1_INVALID_UNIT -19
#define P1_WAIT_ABORTED -20
#define P1_CHILD_QUIT -21
#define P1_NAME_TOO_LONG -22
#define P1_CONTEXT_IN_USE -23
#define P1_LOCK_HELD -24

#endif /* _PHASE1_H */
