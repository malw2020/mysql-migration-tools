/* 
 * File:   File.h
 * Author: malw
 *
 * Created on 2014年4月17日, 上午10:10
 */

#ifndef FILE_H
#define	FILE_H

#include <string>

using namespace std;

class File
{
public:
    File();
    ~File();
    
public:
    static bool exist(string path);
    static bool rename(const string &path, const string &oldName, const string &newName);
};

#endif	/* FILE_H */

