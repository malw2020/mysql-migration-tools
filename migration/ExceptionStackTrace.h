/* 
 * File:   ExceptionStackTrace.h
 * Author: malw
 *
 * Created on 2014年3月24日, 上午9:50
 */

#ifndef EXCEPTIONSTACKTRACE_H
#define	EXCEPTIONSTACKTRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

/***************************************************/
// ExceptionStackTrace: output stack trace infomation
class ExceptionStackTrace
{
private:
    ExceptionStackTrace()
    {
    }

public:
    static ExceptionStackTrace *get_instance();

    void output(FILE *out = stderr, unsigned int max_frames = 63);

private:
    bool splitMessage(const char * rawInfomation, string &fileName, string &beginName, string &offset);

    string buildCommand(char * rawInfomation);

    string getFileAndLineInfo(string cmd);

private:
    static ExceptionStackTrace *_instance;
};

#endif /* EXCEPTIONSTACKTRACE_H */

