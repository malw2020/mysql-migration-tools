/* 
 * File:   directory.h
 * Author: malw
 *
 * Created on 2014年4月11日, 下午2:21
 */

#ifndef DIRECTORY_H
#define	DIRECTORY_H

#include <string>
#include <vector>

using namespace std;

class Directory
{
public:
    Directory();
    ~Directory();
   
public:
    static int getCurrentPath(string &path);
    static int listDir(string strDir, vector<string> &files, bool onlyFile = true);
    
};

#endif	/* DIRECTORY_H */
