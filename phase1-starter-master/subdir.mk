# This is a sample Makefile for Phase 1.
#
#       make            (makes all libraries and all tests)
#		make install	(installs library in ~/lib and headers in ~/include
#
#       make testN      (makes testN)
#       make testN.out  (runs testN and puts output in testN.out)
#       make tests      (makes all testN.out files, i.e. runs all tests)
#       make tests_bg   (runs all tests in the background)
#
#       make testN.v    (runs valgrind on testN and puts output in testN.v)
#       make valgrind   (makes all testN.v files, i.e. runs valgrind on all tests)
#
#       make clean      (removes all files created by this Makefile)

# sh is dash on lectura which breaks things
SHELL = bash 

ifndef PREFIX
        PREFIX = $(HOME)
endif

# Compute the phase from the current working directory.
ifndef PHASE
	PHASE = $(lastword $(subst /, ,$(CURDIR)))
endif

PHASE_UPPER = $(shell tr '[:lower:]' '[:upper:]' <<< $(PHASE))
VERSION = $($(PHASE_UPPER)_VERSION)

# Compile all C files in this directory.
SRCS = $(wildcard *.c)

# Tests are in the "tests" directory.
TESTS = $(patsubst %.c,%,$(wildcard tests/*.c))

# Change this if you want to change the arguments to valgrind.
VGFLAGS = --track-origins=yes --leak-check=full --max-stackframe=100000

# Change this if you need to link against additional libraries (probably not).
LIBS = -lusloss$(USLOSS_VERSION) \
	   -luser$(USLOSS_VERSION) \
	   -ldisk$(USLOSS_VERSION)

LDFLAGS += -L. -L$(PREFIX)/cs452/lib -L$(PREFIX)/lib 

ifeq ($(PHASE), phase1b)
	LIBS += -lphase1a-$(PHASE1A_VERSION)
	LDFLAGS += -L../phase1a
else ifeq ($(PHASE), phase1c)
	LIBS += -lphase1a-$(PHASE1A_VERSION)
	LIBS += -lphase1b-$(PHASE1B_VERSION)
	LDFLAGS += -L../phase1a -L../phase1b
else ifeq ($(PHASE), phase1d)
	LIBS += -lphase1a-$(PHASE1A_VERSION)
	LIBS += -lphase1b-$(PHASE1B_VERSION)
	LIBS += -lphase1c-$(PHASE1C_VERSION)
	LDFLAGS += -L../phase1a -L../phase1b -L../phase1c
endif

LIBS += -l$(PHASE)-$(VERSION) 

# Change this if you want change which flags are passed to the C compiler.
CFLAGS += -Wall -g -std=gnu99 -Werror -DPHASE=$(PHASE_UPPER) -D$(PHASE_UPPER) -DVERSION=$(VERSION) -DDATE="`date`"
#CFLAGS += -DDEBUG

# You shouldn't need to change anything below here. 

TARGET = lib$(PHASE)-$(VERSION).a

INCLUDES = -I. -I.. -I$(PREFIX)/include

ifeq ($(shell uname),Darwin)
	DEFINES += -D_XOPEN_SOURCE
	OS = macOS
	CFLAGS += -Wno-int-to-void-pointer-cast -Wno-extra-tokens -Wno-unused-label -Wno-unused-function
else
	OS = Linux
	CFLAGS += -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-unused-but-set-variable -Wno-unused-function
	LDFLAGS += -static
endif

CFLAGS += -DOS=$(OS)

CC=gcc
LD=gcc
AR=ar   
RANLIB=ranlib 
CFLAGS += $(INCLUDES) $(DEFINES)
COBJS = ${SRCS:.c=.o}
DEPS = ${COBJS:.o=.d}
TSRCS = {$TESTS:=.c}
TOBJS = ${TESTS:=.o}
TDEPS = ${TOBJS:.o=.d}
TOUTS = ${TESTS:=.out}
TVS = ${TESTS:=.v}
STUBS = ../p3/p3stubs.o

# The following is to deal with circular dependencies between the USLOSS and phase1
# libraries. Unfortunately the linkers handle this differently on the two OSes.

ifeq ($(OS), macOS)
	LIBFLAGS = -Wl,-all_load $(LIBS)
else
	LIBFLAGS = -Wl,--start-group $(LIBS) -Wl,--end-group
endif

%.d: %.c
	$(CC) -c $(CFLAGS) -MM -MF $@ $<

all: $(PHASE)

$(PHASE): $(TARGET) $(TESTS)


$(TARGET):  $(COBJS)
	$(AR) -r $@ $^
	$(RANLIB) $@


install: $(TARGET)
	mkdir -p ~/lib
	install $(TARGET) ~/lib

.NOTPARALLEL: tests
tests: $(TOUTS)

# Remove implicit rules so that "make phaseX" doesn't try to build it from phaseX.c or phaseX.o
% : %.c

% : %.o

%.out: %
	./$< 1> $@ --virtual-time 2>&1

$(TESTS):   %: $(TARGET) %.o $(STUBS)
	$(LD) $(LDFLAGS) -o $@ $@.o $(STUBS) $(LIBFLAGS)

clean:
	rm -f $(COBJS) $(TARGET) $(TOBJS) $(TESTS) $(DEPS) $(TDEPS) $(TVS) $(STUBS) *.out tests/*.out tests/*.err

%.d: %.c
	$(CC) -c $(CFLAGS) -MM -MF $@ $<

valgrind: $(TVS)

%.v: %
	valgrind $(VGFLAGS) ./$< 1> $@ 2>&1

./p3/p3stubsTest: $(STUBS) ./p3/p3stubsTest.o
	$(LD) $(LDFLAGS) -o $@ $^

-include $(DEPS) 
-include $(TDEPS)
