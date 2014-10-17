## -*- Makefile -*-
##
##: maliwei
##: 2014-5-7 23:31:05
## Makefile created by Oracle Solaris Studio.

CCC = gcc
CXX = g++
BASICOPTS = -g -rdynamic -Wall -m64
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CXX_H_PATH =  -I/usr/include/mysql 
CXX_LIB_PATH = -L /usr/lib64/mysql

OBJ_DEPEND = ../lib/libmysqlreplication.a ../lib/libtinyxml.a ../lib/libcommon.a
LDLIBS = -lcrypto -lmysqlclient -lpthread -llog4cpp 

AppTarget = ../debug/mysqlreplication

OBJS = main.o replication_patterns.o mysql_adapter.o dispatcher.o \
					
%o: %c
	$(CCC) $(CCFLAGS) $(CXX_H_PATH) -c -o  $@ $<

%o: %cpp
	$(CXX) $(CXXFLAGS) $(CXX_H_PATH) -c -o  $@ $<

$(AppTarget): $(OBJS)	
	$(CXX) $(CXXFLAGS) $(CXX_H_PATH) -o $@ $(OBJS) $(OBJ_DEPEND) $(CXX_LIB_PATH) $(LDLIBS) 

.PHONY:clean 
clean: 
	rm -f *.o  ../debug/mysqlreplication 
	

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

