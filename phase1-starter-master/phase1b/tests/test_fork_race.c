#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include <stdio.h>
#include <tester.h>

/*
 * Tests for race conditions in fork and quit, e.g. are they in a critical section.
 * Repeatedly forks processes from the main process and the alarm interrupt handler.
 */

static int
Quitter(void *arg) 
{
    return (int) arg;
}

void
CreateChild(int id)
{
    int pid, status, rc, child;
    char    name[P1_MAXNAME+1];

    // clean up any children that have quit
    while(1) {
        rc = P1GetChildStatus(&pid, &status);
        if (rc != P1_SUCCESS) {
            break;
        }
    }

    // create a child
    snprintf(name, sizeof(name), "Quitter %d", id);
    rc = P1_Fork(name, Quitter, (void *) id, USLOSS_MIN_STACK, 1, &child);
    TEST(rc, P1_SUCCESS);
}

static int
Parent(void *arg)
{   
    for (int i = 0; i < 100000; i++) {
        CreateChild(i);
    }
    int rc, pid, status;
    rc = P1GetChildStatus(&pid, &status);
    TEST(rc, P1_SUCCESS);
    USLOSS_Console("TEST PASSED.\n");
    return 0;
}
static void
AlarmHandler(int type, void *arg)
{
    static int ticks = 0;
    int rc;

    //USLOSS_Console("tick\n");
    ticks--;
    CreateChild(ticks);
    rc = USLOSS_DeviceOutput(USLOSS_ALARM_DEV, 0, (void *) 1);
    TEST(rc, P1_SUCCESS);
}

static void
DummyHandler(int type, void *arg) {}

void
startup(int argc, char **argv)
{
    int pid;
    int rc;
    P1ProcInit();
    USLOSS_Console("startup\n");
    USLOSS_IntVec[USLOSS_ALARM_INT] = AlarmHandler;
    USLOSS_IntVec[USLOSS_CLOCK_INT] = DummyHandler;
    rc = USLOSS_DeviceOutput(USLOSS_ALARM_DEV, 0, (void *) 1);
    TEST(rc, P1_SUCCESS);
    rc = P1_Fork("Parent", Parent, NULL, USLOSS_MIN_STACK, 6, &pid);
    TEST(rc, P1_SUCCESS);
    // should not return
    FAILED(1,0);
}

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {}
