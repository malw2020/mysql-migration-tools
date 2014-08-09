CCC = g++
CXX = g++
BASICOPTS = -g -rdynamic -Wall -O3 -m64
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCADMIN = 
DOS2UNIX=1


TARGETDIR_test=.

all:${DOS2UNIX}  pig

${DOS2UNIX}:
	dos2unix *.cpp
	
pig:
	cd lib_mysql_replication && make
	cd migration && make
		
clean: 
	cd lib_mysql_replication && make clean
	cd migration && make clean

	rm -f *.o migration

.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

