/* 
 * File:   datetime.h
 * Author: yfmaliwei
 *
 * Created on 2014年4月17日, 上午10:52
 */

#ifndef DATETIME_H
#define	DATETIME_H

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string>

using namespace std;

class Datetime
{
public:
    Datetime();
    ~Datetime();
    
public:
    static bool get_current_datatime(string &datetime);
    
};

#endif	/* DATETIME_H */

