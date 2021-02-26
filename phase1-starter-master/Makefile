# This is the top-level Makefile for Phase 1. 
#       make            (makes all libraries and all tests)
#		make install	(installs all libraries in ~/lib and headers in ~/include
#		make phase1x	(makes library for phase1x)
#		make phase1x-install (installs phase1x library in ~/lib and headers in ~/include
#		make phase1x-tests (makes phase1x and runs tests in phase1x/tests)
#
# The build will look for the phase libraries and USLOSS in the following locations in this order:
#
#		~/cs452/lib	-- solutions for phase libraries we've provided
#		~/lib -- libraries you've built and installed
#		<source directory> -- development versions of libraries
#
# For example, phase1b needs libphase1a-X.Y.a, where X.Y is the version number. 
# The build will first look in ~/cs452/lib, then ~/lib, then the phase1a
# subdirectory to find this library. Similarly it will look for the USLOSS library 
# in these locations and should find it in ~/lib if you installed USLOSS correctly.
#


TOP_PHASE = phase1
SUBDIRS=$(wildcard $(TOP_PHASE)[a-d])
INSTALLS=$(patsubst %, %-install, $(SUBDIRS))
TESTS=$(patsubst %, %-tests, $(SUBDIRS))

HDRS=$(TOP_PHASE).h $(TOP_PHASE)Int.h

.PHONY: $(SUBDIRS) all clean install subdirs $(INSTALLS) $(TESTS)

all: $(SUBDIRS)

subdirs: $(SUBDIRS)

clean: $(SUBDIRS)
	rm -f term*.out

install: $(INSTALLS)

tests: $(TESTS)

tar:
	(cd ..; gnutar cvzf ~/Downloads/$(TOP_PHASE)-starter.tgz --exclude=.git --exclude="*.dSYM" $(TOP_PHASE)-starter)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(INSTALLS):
	$(MAKE) -C $(patsubst %-install, %, $@) install
	install $(HDRS) ~/include

$(TESTS):
	$(MAKE) -C $(patsubst %-tests, %, $@) tests
