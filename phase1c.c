
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <usloss.h>
#include <phase1Int.h>

#define CHECKKERNEL() \
    if ((USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE) == 0) USLOSS_IllegalInstruction()

#define FREE 0
#define BUSY 1

typedef struct Lock
{
    int         inuse;
    char        name[P1_MAXNAME];
    int         state; // BUSY or FREE
    int         pid; 
    // more fields here
} Lock;

static Lock locks[P1_MAXLOCKS];

void 
P1LockInit(void) 
{
    CHECKKERNEL();
    P1ProcInit();
    for (int i = 0; i < P1_MAXLOCKS; i++) {
        locks[i].inuse = FALSE;
    }
}


int P1_LockCreate(char *name, int *lid)
{
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // disable interrupts
    // check parameters
    // find an unused Lock and initialize it
    // restore interrupts
    return result;
}

int P1_LockFree(int lid) 
{
    int     result = P1_SUCCESS;
    CHECKKERNEL();
    // disable interrupts
    // check if any processes are waiting on lock
    // mark lock as unused and clean up any state
    // restore interrupts
    return result;
}

int P1_Lock(int lid) 
{
    int result = P1_SUCCESS;
    CHECKKERNEL();
    /*********************

    Pseudo-code from the lecture notes.

    while(1) {
          DisableInterrupts();
          if (lock->state == FREE) {
              lock->state = BUSY;
              break;
          }
          Mark process BLOCKED, add to lock->q
          RestoreInterrupts();
          Dispatcher();
    }
    RestoreInterrupts();

    *********************/

    return result;
}


int P1_Unlock(int lid) 
{
    int result = P1_SUCCESS;
    CHECKKERNEL();
    /*********************

      DisableInterrupts();
      lock->state = FREE;
      if (lock->q is not empty) {
          Remove process from lock->q, mark READY
          Dispatcher();
      }
      RestoreInterrupts();

    *********************/
    return result;
}

int P1_LockName(int lid, char *name, int len) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // more code here
    return result;
}

/*
 * Condition variable functions.
 */

typedef struct Condition
{
    int         inuse;
    char        name[P1_MAXNAME];
    int         lock;  // lock associated with this variable
    // more fields here
} Condition;

static Condition conditions[P1_MAXCONDS];

void P1CondInit(void) {
    CHECKKERNEL();
    P1LockInit();
    for (int i = 0; i < P1_MAXCONDS; i++) {
        conditions[i].inuse = FALSE;
    }
}

int P1_CondCreate(char *name, int lid, int *vid) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // more code here
    return result;
}

int P1_CondFree(int vid) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // more code here
    return result;
}


int P1_Wait(int vid) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    /*********************

      DisableInterrupts();
      Confirm lock is held
      cv->waiting++;
      Release(cv->lock);
      Make process BLOCKED, add to cv->q
      Dispatcher();
      Acquire(cv->lock);
      RestoreInterrupts();

    *********************/
    return result;
}

int P1_Signal(int vid) {
    int result = P1_SUCCESS;
        CHECKKERNEL();
    /*********************

      DisableInterrupts();
      Confirm lock is held 
      if (cv->waiting > 0) {
        Remove process from cv->q, make READY
        cv->waiting--;
        Dispatcher();
      }
      RestoreInterrupts();
    *********************/    
    return result;
}

int P1_Broadcast(int vid) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    /*********************
      DisableInterrupts();
      Confirm lock is held 
      while (cv->waiting > 0) {
        Remove process from cv->q, make READY
        cv->waiting--;
        Dispatcher();
      }
      RestoreInterrupts();
    *********************/    
    return result;
}

int P1_NakedSignal(int vid) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // more code here
    return result;
}

int P1_CondName(int vid, char *name, int len) {
    int result = P1_SUCCESS;
    CHECKKERNEL();
    // more code here
    return result;
}
