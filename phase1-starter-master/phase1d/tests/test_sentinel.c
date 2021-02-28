/* 
 * Tests that sentinal halts when there are no runnable processes. P2_Startup forks a child,
 * does some lock and condition variable operations, then both quit at which point the sentinel
 * should halt the system.
 */

#include "phase1.h"
#include "usloss.h"
#include <stdlib.h>
#include "tester.h"
static int lock;
static int cond;

int Child(void *arg){
   USLOSS_Console("Child: Child running.\n");
   int rc = P1_Lock(lock);
   TEST(rc, P1_SUCCESS);
   USLOSS_Console("Child: Child Signaling.\n");
   rc = P1_Signal(cond);
   TEST(rc, P1_SUCCESS);
   rc = P1_Unlock(lock);
   TEST(rc, P1_SUCCESS);

   USLOSS_Console("Child: Child quitting.\n");
   return 0;
}


int P2_Startup(void *notused) 
{
    int rc;
    int pid;
    USLOSS_Console("P2_Startup: Creating lock and cond.\n");
    rc = P1_LockCreate("lock", &lock);
    TEST(rc, P1_SUCCESS);
    rc = P1_CondCreate("cond", lock, &cond);
    TEST(rc, P1_SUCCESS);

    USLOSS_Console("P2_Startup: Forks Child with priority 3.\n");
    rc = P1_Fork("Child", Child, NULL, USLOSS_MIN_STACK, 3, &pid);
    TEST(rc, P1_SUCCESS);
       
    rc = P1_Lock(lock);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("P2_Startup: Waiting on cond.\n");
    rc = P1_Wait(cond);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("P2_Startup: Returned from Wait.\n");
    rc = P1_Unlock(lock);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("P1_Startup: quitting.\n");
    return 0;
}

void test_setup(int argc, char **argv) {
    // Do nothing.
}

void test_cleanup(int argc, char **argv) {
    // Do nothing.
}
void finish(int argc, char **argv) {
    extern int finish_status;
    TEST_FINISH(finish_status, 0);
    PASSED_FINISH();
}
