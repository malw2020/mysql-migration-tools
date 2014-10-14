## -*- Makefile -*-
##
##: maliwei
##: 2014-5-7 23:31:05
## Makefile created by Oracle Solaris Studio.

CCC = g++
CXX = g++
BASICOPTS = -g -rdynamic -Wall -O3 -m64
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCADMIN = 
DOS2UNIX=1


TARGETDIR_test=.

all: pig

#${DOS2UNIX}:
#	dos2unix *.cpp
	
pig:
	cd lib_mysql_replication && make
	cd lib_tinyxml && make
	cd lib_common && make
	cd migration && make	
		
clean: 
	cd lib_mysql_replication && make clean
	cd lib_tinyxml && make clean
	cd lib_common && make clean
	cd migration && make clean

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

