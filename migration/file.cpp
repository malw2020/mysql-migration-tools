#include "file.h"

#include <stdio.h>
#include <unistd.h>

File::File()
{ 
}
 
File::~File()
{
}
    
bool File::exist(string path)
{
    if (access(path.c_str(), F_OK) == 0)
        return true;
    
    return false;
}

bool File::rename(const string &path, const string &oldName, const string &newName)
{
    string oldPath = path + "/" + oldName;
    string newPath = path + "/" + newName;
    if(0 == ::rename(oldPath.c_str(), newPath.c_str())) {
        return true;
    }
    
    return false;
}
