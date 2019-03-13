# release build - make DEBUG=0
# debug build (default) - make DEBUG=1


OPENCV ?= 0
DEBUG  ?= 1

CC=gcc
CXX=g++
RM=rm -f

CPPFLAGS= -std=c++11 
LDFLAGS=
LDLIBS=

ifeq ($(OPENCV), 1) 
	CPPFLAGS+= -DVS_USE_OPENCV $(shell pkg-config --cflags opencv) 
	LDLIBS+=$(shell pkg-config --libs opencv) 
endif

ifeq ($(DEBUG), 1)
	CPPFLAGS +=-g -Wall -pedantic -fwrapv
	LDFLAGS +=-g
else
	CPPFLAGS += -O3 -flto -DNDEBUG
endif

SRCS=$(shell find ./source -name "*.cpp")
SRCS+=$(shell find ./examples/unit -name "*.cpp")

OBJS=$(subst .cpp,.o,$(SRCS))

all: unit_tests panorama opticalflow

opticalflow: $(OBJS) ./examples/opticalflow.cpp
	$(CXX) $(LDFLAGS) -o opticalflow $^ $(LDLIBS)

panorama: $(OBJS) ./examples/panorama.cpp
	$(CXX) $(LDFLAGS) -o panorama $^ $(LDLIBS)

unit_tests: $(OBJS) ./examples/unit_tests.cpp
	$(CXX) $(LDFLAGS) -o unit_tests $^ $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) unit_tests
	$(RM) panorama
	$(RM) opticalflow
	
distclean: clean
	$(RM) *~ .depend
