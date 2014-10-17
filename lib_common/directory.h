/* 
 * File:   directory.h
 * Author: malw
 *
 * Created on 2014年4月11日, 22:21
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
    static int get_current_path(string &path);
    static int list_dir(string strDir, vector<string> &files, bool onlyFile = true);
    
};

#endif	/* DIRECTORY_H */
