# release build - make DEBUG=0
# debug build (default) - make DEBUG=1

CC=gcc
CXX=g++
RM=rm -f

CPPFLAGS= -std=c++11 
LDFLAGS=
LDLIBS=

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CPPFLAGS +=-g -Wall -pedantic -fwrapv
	LDFLAGS +=-g
else
	CPPFLAGS += -O3 -flto
    CFLAGS +=-DNDEBUG
endif

SRCS=$(shell find ./source -name "*.cpp")
SRCS+=$(shell find ./examples -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: unit_tests panorama

panorama: $(OBJS) ./panorama.cpp
	$(CXX) $(LDFLAGS) -o panorama $^ $(LDLIBS)

unit_tests: $(OBJS) ./unit_tests.cpp
	$(CXX) $(LDFLAGS) -o unit_tests $^ $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) unit_tests
	$(RM) panorama

distclean: clean
	$(RM) *~ .depend
