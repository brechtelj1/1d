#include <stdio.h>	
#include "phase1.h"
#include "tester.h"
#include <stdlib.h>
#include <assert.h> 
#include <libdisk.h>
	
int 
P2_Startup(void *arg) {
	int rc;
	int status;
	int tracks;
	USLOSS_DeviceRequest req;

	// get the number of tracks
	req.opr = USLOSS_DISK_TRACKS; 
	req.reg1 = (void *) &tracks; 
	rc = USLOSS_DeviceOutput(USLOSS_DISK_DEV, 0, &req); 
	TEST(rc, USLOSS_ERR_OK);	
	rc = P1_DeviceWait(USLOSS_DISK_DEV, 0, &status); 
	TEST(rc, P1_SUCCESS);
	TEST(status, USLOSS_DEV_READY); 

	// seek to the last track

	req.opr = USLOSS_DISK_SEEK; 
	req.reg1 = (void *) tracks-1; 
	rc = USLOSS_DeviceOutput(USLOSS_DISK_DEV, 0, &req); 
	TEST(rc, USLOSS_ERR_OK);	
	rc = P1_DeviceWait(USLOSS_DISK_DEV, 0, &status); 
	TEST(rc, P1_SUCCESS);
	TEST(status, USLOSS_DEV_READY); 

	// seek back to the first track

	req.opr = USLOSS_DISK_SEEK; 
	req.reg1 = (void *) 0; 
	rc = USLOSS_DeviceOutput(USLOSS_DISK_DEV, 0, &req); 
	TEST(rc, USLOSS_ERR_OK);	
	rc = P1_DeviceWait(USLOSS_DISK_DEV, 0, &status); 
	TEST(rc, P1_SUCCESS);
	TEST(status, USLOSS_DEV_READY); 

	PASSED();
	return 0;	
	
}

void test_setup(int argc, char **argv) {
	int rc;

    DeleteAllDisks();
    // create disk 0 with 3 tracks
    rc = Disk_Create(NULL, 0, 3);
    assert(rc == 0);
}

void test_cleanup(int argc, char **argv) {
	DeleteAllDisks();
}

void finish(int argc, char **argv) {}
