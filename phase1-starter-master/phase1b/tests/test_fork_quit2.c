#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include <stdlib.h>
#include <tester.h>

// Tests quitting the Parent process when its children have not yet quit.
// Child should be inherited by the first process.

static int
Output(void *arg) 
{
    char *msg = (char *) arg;

    USLOSS_Console("%s", msg);
    P1_Quit(11);
    // should not return
    FAILED(1,0);
    return 0;
}

static int
Parent(void *arg)
{   
    int     pid = -1;
    int     rc;
    int     status = -1;
    int     childPid;
    char    *msg = (char *) arg;

    // the parent has higher priority so it continues running after forking
    rc = P1_Fork("Child", Output, msg, USLOSS_MIN_STACK, 2, &childPid);
    TEST(rc, P1_SUCCESS);

    // the parent should see that the child has not quit
    rc = P1GetChildStatus(&pid, &status);
    TEST(rc, P1_NO_QUIT);
    TEST(pid, -1); // pid should not be set
    TEST(status, -1); // same for status

    // after the parent quits, the child is adopted by P6Proc
    return 42;
}

int P6Proc(void *arg)
{
    int pid;
    // fork parent with the highest priority
    int rc = P1_Fork("Hello", Parent, "Hello World!\n", USLOSS_MIN_STACK, 1, &pid);
    TEST(rc, P1_SUCCESS);

    int rv, status, childpid;
    // clean up Parent
    rv = P1GetChildStatus(&childpid, &status);
    TEST(rv, P1_SUCCESS);
    TEST(childpid, 1);
    TEST(status, 42);

    // clean up Child
    rv = P1GetChildStatus(&childpid, &status);
    TEST(rv, P1_SUCCESS);
    TEST(childpid, 2);
    TEST(status, 11);
    PASSED();
    return 0;
}

void
startup(int argc, char **argv)
{
    int pid;
    int rc;
    P1ProcInit();
    USLOSS_Console("startup\n");
    rc = P1_Fork("P6Proc", P6Proc, "Hello World!\n", USLOSS_MIN_STACK, 6, &pid);
    TEST(rc, P1_SUCCESS);
    // should not return
    FAILED(1,0);
}

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {}
