CC=gcc
CXX=g++
RM=rm -f

CPPFLAGS=-g -std=c++11 -Wall -pedantic
LDFLAGS=-g
LDLIBS=

SRCS=$(shell find ./source -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: image_io

image_io: $(OBJS) ./examples/image_io.cpp
	$(CXX) $(LDFLAGS) -o image_io $^ $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) image_io

distclean: clean
	$(RM) *~ .depend
