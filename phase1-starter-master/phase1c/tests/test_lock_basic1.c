
 /* Tests basic lock functionality. Worker 1 is forked at priority 6, acquires
 * lock and forks Worker 2 a higher priority. Worker 2 should then block on lock 
 * until Worker 1 releases lock, then continue. The "flag" variable is to 
 * test that the workers ran in the correct order.
 *
 * Expected output:

    Worker 1 starting.
    Worker 1 acquiring the lock.
    Worker 1 now has the lock.
    Worker 1 forking Worker 2.
    Worker 2 starting.
    Worker 2 acquiring the lock.
    Worker 1 incrementing flag.
    Worker 1 releasing the lock.
    Worker 2 now has the lock.
    Worker 2 testing value of flag.
    Worker 2 modifying value of flag.
    Worker 2 releasing the lock.
    Worker 2 done.
    Worker 1 done.
    No runnable processes, halting.
    TEST PASSED.
 */
 
#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include "tester.h"

#define WORKER_1_PRIORITY 3
#define WORKER_2_PRIORITY (WORKER_1_PRIORITY - 1)

static int flag = 0;
static int worker2 = FALSE;

int Worker2(void *arg)
{
    int lock = (int) arg;
    
    USLOSS_Console("Worker 2 starting.\n");
    worker2 = TRUE;

    USLOSS_Console("Worker 2 acquiring the lock.\n");
    int rc = P1_Lock(lock);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("Worker 2 now has the lock.\n");

    USLOSS_Console("Worker 2 testing value of flag.\n");
    TEST(flag, 1); // worker 1 should have incremented it

    USLOSS_Console("Worker 2 modifying value of flag.\n");
    flag *= 2;

    USLOSS_Console("Worker 2 releasing the lock.\n");
    rc = P1_Unlock(lock);
    TEST(rc, P1_SUCCESS);

    USLOSS_Console("Worker 2 done.\n");
    return 0;
}

int Worker1(void *arg)
{
    int lock = (int) arg;
    int pid;

    USLOSS_Console("Worker 1 starting.\n");
    USLOSS_Console("Worker 1 acquiring the lock.\n");
    int rc = P1_Lock(lock);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("Worker 1 now has the lock.\n");

    USLOSS_Console("Worker 1 forking Worker 2.\n");
    rc = P1_Fork("Worker 2", Worker2, (void *) lock, USLOSS_MIN_STACK, WORKER_2_PRIORITY, &pid);
    TEST(rc, P1_SUCCESS);
    // Worker 2 should already have run as it is higher priority, but it should not
    // have modified the flag because it should have waited on the lock
    TEST(worker2, TRUE);
    TEST(flag, 0);

    USLOSS_Console("Worker 1 incrementing flag.\n");
    flag++;

    USLOSS_Console("Worker 1 releasing the lock.\n");
    rc = P1_Unlock(lock);
    TEST(rc, P1_SUCCESS);

    // Worker 2 should have modified the flag.
    TEST(flag, 2);

    USLOSS_Console("Worker 1 done.\n");
    return 0;
}

int
Init(void *arg) 
{
    int pid;
    int lock = (int) arg;
    int rc = P1_Fork("Worker1", Worker1, (void *) lock, USLOSS_MIN_STACK, WORKER_1_PRIORITY, &pid);
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
    int lock;

    P1LockInit();
    rc = P1_LockCreate("lock", &lock);
    TEST(rc, P1_SUCCESS);

    rc = P1_Fork("Init", Init, (void *) lock, USLOSS_MIN_STACK, 6, &pid);
    assert(rc == P1_SUCCESS);
}

void dummy(int type, void *arg) {};

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {
    TEST_FINISH(flag, 2);
    PASSED_FINISH();
}

