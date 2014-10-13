## -*- Makefile -*-
##
##: yfmaliwei
##: 2014-5-7 15:31:05
## Makefile created by Oracle Solaris Studio.

CCC = gcc
CXX = g++
BASICOPTS = -g -fPIC -rdynamic -Wall -m64
BASICLIBOPTS = -shared -fPIC
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCLIBFLAGS = $(BASICLIBOPTS)
CXXLIBFLAGS = $(BASICLIBOPTS)
CXX_H_PATH = -I/usr/include/mysql
CXX_LIB_PATH = -L /usr/lib64/mysql

OBJ_STATIC = ../lib/libmysqlreplication.a
OBJ_SHARE = ../lib/libmysqlreplication.so

LDLIBS = -lcrypto -lmysqlclient -lpthread

all: ../lib/libmysqlreplication.a ../lib/libmysqlreplication.so

OBJS = access_method_factory.o \
					basic_content_handler.o \
					basic_transaction_parser.o \
					binary_log.o \
					binlog_driver.o \
					binlog_event.o \
					field_iterator.o \
					file_driver.o \
					protocol.o \
					resultset_iterator.o \
					row_of_fields.o \
					tcp_driver.o \
					utilities.o \
					value.o \
					
%o: %c
	$(CCC) $(CCFLAGS) $(CXX_H_PATH) -c -o  $@ $<

%o: %cpp
	$(CXX) $(CXXFLAGS) $(CXX_H_PATH) -c -o  $@ $<
	
$(OBJ_STATIC): $(OBJS)
	ar rv $@ ${OBJS}

$(OBJ_SHARE): $(OBJS)
	$(CXX)  $(CXXLIBFLAGS) -Wl,-rpath, $(CXX_H_PATH) -o $@ $< $(CXX_LIB_PATH) $(LDLIBS) 

.PHONY:clean 
clean: 
	rm -f *.o ../lib/libmysqlreplication.a ../lib/libmysqlreplication.so
	

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux
