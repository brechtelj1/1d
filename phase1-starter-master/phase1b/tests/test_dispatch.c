#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>
#include "usloss.h"
#include <stdlib.h>
#include <tester.h>

/*
 * Tests calling P1Dispatch(TRUE) with 3 processes of the same priority.
 * They should run round-robin in this order: Parent, Child0, Child1.
 */

int x = 0;

int Child0(void *arg)
{
    USLOSS_Console("Child0: running.\n");   
    TEST(x, 1);
    x = 2;
    P1Dispatch(TRUE); // should run child1 next
    USLOSS_Console("Child0: running again.\n");   
    TEST(x, 4);
    x = 5;
    USLOSS_Console("Child0: quitting.\n");
    P1_Quit(12);    
    return 0;
}

int Child1(void *arg)
{
    USLOSS_Console("Child1: running.\n");
    TEST(x, 2);
    x = 3;
    P1Dispatch(TRUE); // should run Parent next
    USLOSS_Console("Child1: running again.\n");   
    TEST(x, 5);
    x = 6;
    USLOSS_Console("Child1: quitting.\n");
    P1_Quit(13);
    return 0;
}


static int
Output(void *arg) 
{
    char *msg = (char *) arg;
    int val, child;
    USLOSS_Console("Parent: %s", msg);
  
    USLOSS_Console("Parent: forking Child0 with priority 3.\n"); 
    val = P1_Fork("Child", Child0, (void *) 0, USLOSS_MIN_STACK, 3, &child);
    TEST(val, P1_SUCCESS);

    USLOSS_Console("Parent: forking Child1 with priority 3.\n"); 
    val = P1_Fork("Child2", Child1, (void *) 0, USLOSS_MIN_STACK, 3, &child);
    TEST(val, P1_SUCCESS);
    
    // After calling P1Dispatch(TRUE), Parent is placed at the back of the ready queue
    USLOSS_Console("Parent: calling P1Dispatch(TRUE)\n");
    x = 1;
    P1Dispatch(TRUE); // should run Child0 next
    USLOSS_Console("Parent: running again\n");
    TEST(x, 3);
    x = 4;
    P1Dispatch(TRUE); // should run Child0 again
    
    USLOSS_Console("Parent: cleaning up children\n");
    // Clean up Child 0
    int pid,  status;
    val = P1GetChildStatus(&pid, &status);
    TEST(val, P1_SUCCESS);
    TEST(pid, 2);
    TEST(status, 12);

    // Clean up Child 1
    val = P1GetChildStatus(&pid, &status);
    TEST(val, P1_SUCCESS);
    TEST(pid, 3);
    TEST(status, 13);

    TEST(x, 6);
    USLOSS_Console("Parent: quitting.\n");
    PASSED();
    // should not return
    FAILED(1,0);
    return 0;
}

int P6Proc(void *arg)
{
    int pid;
    USLOSS_Console("P6Proc: forking Parent with priority 3.\n");
    int rc = P1_Fork("Hello", Output, "Hello World!\n", USLOSS_MIN_STACK, 3, &pid);
    TEST(rc, P1_SUCCESS);
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
    // P1_Fork should not return
    FAILED(1, 0);
}

void test_setup(int argc, char **argv) {}

void test_cleanup(int argc, char **argv) {}

void finish(int argc, char **argv) {}
