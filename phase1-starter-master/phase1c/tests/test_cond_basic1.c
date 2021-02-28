/*
 * Tests basic functionality of the condition variables. Worker 1 forks
 * Worker 2 which runs at a lower priority, then waits on a condition
 * variable. Worker 2 should then run and signal Worker 1, which should
 * then quit then Worker 2 should quit. The "flag" variable is to make
 * sure the processes run in the correct order. The Init process is
 * to prevent the first process from quitting with children during the test.
 *
 * Expected output:
    Worker 1 starting.
    Worker 1 acquiring the lock.
    Worker 1 now has the lock.
    Worker 1 forking Worker 2.
    Worker 1 waiting.
    Worker 2 starting.
    Worker 2 acquiring the lock.
    Worker 2 now has the lock.
    Worker 2 setting flag.
    Worker 2 signaling worker 1.
    Worker 2 releasing the lock.
    Worker 1 running again.
    Worker 1 modifying flag.
    Worker 1 releasing the lock.
    Worker 1 done.
    Worker 2 done.
    No runnable processes, halting.
    TEST PASSED.
 */

#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include "tester.h"

static int flag = 0;
static int lid;
static int vid;

#define WORKER_1_PRIORITY 2
#define WORKER_2_PRIORITY (WORKER_1_PRIORITY + 1)

int Worker2(void *arg)
{    
    USLOSS_Console("Worker 2 starting.\n");

    USLOSS_Console("Worker 2 acquiring the lock.\n");
    int rc = P1_Lock(lid);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("Worker 2 now has the lock.\n");
   
    USLOSS_Console("Worker 2 setting flag.\n");
    flag = 1;
    // wake up worker 1
    USLOSS_Console("Worker 2 signaling worker 1.\n");
    rc = P1_Signal(vid);
    TEST(rc, P1_SUCCESS);

    // give control back to worker 1
    USLOSS_Console("Worker 2 releasing the lock.\n");
    rc = P1_Unlock(lid);
    TEST(rc, P1_SUCCESS);

    // see changes from worker 1
    TEST(flag, 2);
    flag = 3;

    USLOSS_Console("Worker 2 done.\n");
    return 0;
}

int Worker1(void *arg)
{
    int pid;

    USLOSS_Console("Worker 1 starting.\n");
    USLOSS_Console("Worker 1 acquiring the lock.\n");
    int rc = P1_Lock(lid);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("Worker 1 now has the lock.\n");

    //creating Worker 2 of lower priority
    USLOSS_Console("Worker 1 forking Worker 2.\n");
    rc = P1_Fork("Worker 2", Worker2, NULL, USLOSS_MIN_STACK, WORKER_2_PRIORITY, &pid);
    TEST(rc, P1_SUCCESS);


    //wait for child to signal
    USLOSS_Console("Worker 1 waiting.\n");
    rc = P1_Wait(vid);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("Worker 1 running again.\n");

    // make sure Worker 2 set the flag
    TEST(flag, 1);

    USLOSS_Console("Worker 1 modifying flag.\n");
    flag *= 2;

    USLOSS_Console("Worker 1 releasing the lock.\n");
    rc = P1_Unlock(lid);
    TEST(rc, P1_SUCCESS);

    USLOSS_Console("Worker 1 done.\n");
    return 0;
}

int
Init(void *arg) 
{
    int pid;
    int rc = P1_Fork("Worker1", Worker1, NULL, USLOSS_MIN_STACK, WORKER_1_PRIORITY, &pid);
    assert(rc == P1_SUCCESS);

    // clean up children
    while(1) {
        int pid, status, rc;
        rc = P1GetChildStatus(&pid, &status);
        if (rc == P1_NO_CHILDREN) {
            break;
        }
    }
    return 0;
}

void
startup(int argc, char **argv)
{
    int pid;
    int rc;

    P1CondInit();
    rc = P1_LockCreate("lock", &lid);
    TEST(rc, P1_SUCCESS);

    rc = P1_CondCreate("cond", lid, &vid);
    TEST(rc, P1_SUCCESS);

    rc = P1_Fork("Init", Init, NULL, USLOSS_MIN_STACK, 6, &pid);
    assert(rc == P1_SUCCESS);
}

void dummy(int type, void *arg) {};

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {
    TEST_FINISH(flag, 3);
    PASSED_FINISH();
}
