## -*- Makefile -*-
##
##: maliwei
##: 2014-10-22 22:56:05
## Makefile created by Oracle Solaris Studio.

CCC = gcc
CXX = g++
BASICOPTS = -g -fPIC -rdynamic -Wall -m64
BASICLIBOPTS = -shared -fPIC
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCLIBFLAGS = $(BASICLIBOPTS)
CXXLIBFLAGS = $(BASICLIBOPTS)
CXX_H_PATH =
CXX_LIB_PATH =

OBJ_STATIC = ../lib/libmeta.a

OBJ_DEPEND = ../lib/libtinyxml.a ../lib/libcommon.a
LDLIBS = -llog4cpp 

all: ../lib/libmeta.a

OBJS = schema.o \
	
					
%o: %c
	$(CCC) $(CCFLAGS) $(CXX_H_PATH) -c -o  $@ $<

%o: %cpp
	$(CXX) $(CXXFLAGS) $(CXX_H_PATH) -c -o  $@ $<
	
$(OBJ_STATIC): $(OBJS)
	ar rv $@ ${OBJS}

.PHONY:clean 
clean: 
	rm -f *.o ../lib/libmeta.a ../lib/libmeta.so
	

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

