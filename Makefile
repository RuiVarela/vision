CC=gcc
CXX=g++
RM=rm -f

CPPFLAGS=-g -std=c++11 -Wall -pedantic
LDFLAGS=-g
LDLIBS=

SRCS=$(shell find ./source -name "*.cpp")
SRCS+=$(shell find ./examples -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: unit_tests

unit_tests: $(OBJS) ./unit_tests.cpp
	$(CXX) $(LDFLAGS) -o unit_tests $^ $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) unit_tests

distclean: clean
	$(RM) *~ .depend
