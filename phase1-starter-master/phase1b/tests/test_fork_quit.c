#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include <stdlib.h>
#include <tester.h>

// Tests forking and quitting
// Child calls P1_Quit(11)
// Parent returns 12, which should have the same effect as calling P1_Quit(12)

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
    int     pid;
    int     rc;
    int     status;
    int     child;
    char    *msg = (char *) arg;

    // child has higher priority and should run to completion.
    rc = P1_Fork("Child", Output, msg, USLOSS_MIN_STACK, 1, &child);
    TEST(rc, P1_SUCCESS);

    rc = P1GetChildStatus(&pid, &status);
    TEST(rc, P1_SUCCESS);
    TEST(pid, child);
    TEST(status, 11);
    return 12;
}

int P6Proc(void *arg){
    int pid;
    int rc = P1_Fork("Parent", Parent, "Hello World!\n", USLOSS_MIN_STACK, 2, &pid);
    TEST(rc, P1_SUCCESS);

    int rv, status, childpid;
    rv = P1GetChildStatus(&childpid, &status);
    TEST(rv, P1_SUCCESS);
    TEST(childpid, 1);
    TEST(status, 12);
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
    FAILED(1, 0);
}

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {}
