#include "directory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define  MAX_PATH 260

int Directory::get_current_path(string &path)
{
    char buf[MAX_PATH] = {'\0'};
    char pidfile[64];
    int bytes;
    int fd;

    sprintf(pidfile, "/proc/%d/cmdline", getpid());

    fd = open(pidfile, O_RDONLY, 0);
    bytes = read(fd, buf, 256);
    close(fd);
         
    char * p = &buf[strlen(buf)];
    do {
	*p = '\0';
	p--;
    } while( '/' != *p );
    
    path = buf;

    return 0;
}

int Directory::list_dir(string strDir, vector<string> &files, bool onlyFile)
{
    struct dirent *entry;
    DIR *dir = opendir(strDir.c_str());
    if (dir == NULL) {
       perror("opendir");
       return -1;
    }
   
    while((entry = readdir(dir))) {
        string name = entry->d_name;
        if(onlyFile && name == "." || name == "..")
            continue;
        
        files.push_back(entry->d_name);
    }
        
 
    closedir(dir);

    return 0;
}
