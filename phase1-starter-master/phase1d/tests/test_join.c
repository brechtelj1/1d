/*
 * A stress test for join.
 */
#include "phase1.h"
#include <assert.h>
#include <stdio.h>
#include "tester.h"
int Child(void *arg) {
    return (int) arg;
}

int P2_Startup(void *notused)
{
    #define NUM 40
    int status = 0;
    int rc;
    int pids[NUM];
    P1_ProcInfo info;
    int pid, child;

    rc = P1_GetProcInfo(P1_GetPid(), &info);

    // test join after child quits

    rc = P1_Fork("Higher", Child, (void *) 42, USLOSS_MIN_STACK, info.priority-1, &pid);
    TEST(rc, P1_SUCCESS);
    rc = P1_Join(&child, &status);
    TEST(rc, P1_SUCCESS);
    TEST(pid, child);
    TEST(status, 42);

    // test join before child quits

    rc = P1_Fork("Lower", Child, (void *) 43, USLOSS_MIN_STACK, info.priority+1, &pid);
    TEST(rc, P1_SUCCESS);
    rc = P1_Join(&child, &status);
    TEST(rc, P1_SUCCESS);
    TEST(pid, child);
    TEST(status, 43);


    // repeatedly create lots of children, make sure join returns the proper pid and status

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < NUM; j++) {
            rc = P1_Fork(MakeName("Child ", j), Child,  (void *) j, USLOSS_MIN_STACK, 
                         info.priority+1, &pids[j]);
            TEST(rc, P1_SUCCESS);
        }
        for (int j = 0; j < NUM; j++) {
            int pid;
            rc = P1_Join(&pid, &status);
	        TEST(rc, P1_SUCCESS);
            int found = FALSE;
            for (int k = 0; k < NUM; k++) {
                if (pids[k] == pid) {
                    found = TRUE;
                    TEST(status, k);
                    pids[k] = -1;
                    break;
                }
            }
            TEST(found, TRUE);
        }
    }
    PASSED();
    return status;
}

void test_setup(int argc, char **argv) {
    // Do nothing.
}

void test_cleanup(int argc, char **argv) {
    // Do nothing.
}
void finish(int argc, char **argv) {}
