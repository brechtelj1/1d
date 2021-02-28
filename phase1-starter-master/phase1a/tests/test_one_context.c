#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include "tester.h"

static void
Output(void *arg) 
{
    char *msg = (char *) arg;

    USLOSS_Console("%s", msg);
    PASSED();
    USLOSS_Halt(0);
}

void
startup(int argc, char **argv)
{
    int cid;
    int rc;
    P1ContextInit();
    rc = P1ContextCreate(Output, "Hello World!\n", USLOSS_MIN_STACK, &cid);
    TEST(rc, P1_SUCCESS);
    rc = P1ContextSwitch(cid);
    // should not return
    TEST(rc, P1_SUCCESS);
    // if we get here we failed the test.
    FAILED(1,0);
}

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {}