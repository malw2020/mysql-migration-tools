/* 
 * File:   SysException.h
 * Author: malw
 *
 * Created on 2014年3月24日, 上午10:04
 */

#ifndef SYSEXCEPTION_H
#define	SYSEXCEPTION_H

#include "ExceptionStackTrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <exception>
#include <iostream>
#include <string>

using namespace std;

/***************************************************/
// ExceptionTracer: 异常对象构造函数中生成一个堆栈跟踪
class ExceptionTracer
{
public:
    ExceptionTracer()
    {
	ExceptionStackTrace::get_instance()->output();
    }
};

/***************************************************/
// SignalExceptionClass，提供了表示内核可能发出信号的 C++ 异常的抽象
// SignalTranslator: 用来实现到 C++ 异常的转换
 template <class SignalExceptionClass> class SignalTranslator
 {
 private:
     class SingleTonTranslator
     {
     public:
        SingleTonTranslator()
        {
            signal(SignalExceptionClass::GetSignalNumber(), SignalHandler);
        }
         
        static void SignalHandler(int)
        {
            throw SignalExceptionClass();
        }
    };
     
public:
    SignalTranslator()
    {
        static SingleTonTranslator s_objTranslator;
    }
 };

/***************************************************/
// SegmentationFaultException: Detect SIGSEGV Fault
 class SegmentationFaultException : public ExceptionTracer, public exception
 {
 public:
     static int GetSignalNumber() {return SIGSEGV;}
 };
 
/***************************************************/
// SegmentationFaultException: Detect SIGFPE Fault
class FloatingPointFaultException : public ExceptionTracer, public exception
{
public:
    static int GetSignalNumber() {return SIGFPE;}
};
 
class ExceptionHandler
{
private:
    class SingleTonHandler
    {
    public:
        SingleTonHandler()
        {
            set_terminate(Handler);
        }
        static void Handler()
        {
            // Exception from construction/destruction of global variables
            try
            {
                // re-throw
                throw;
            }
            catch (SegmentationFaultException &)
            {
                cout << "SegmentationFaultException Was Detected." << endl;
            }
            catch (FloatingPointFaultException &)
            {
                cout << "FloatingPointFaultException Was Detected." << endl;
            }
            catch (...)
            {
                cout << "Unknown Exception" << endl;
            }
            //if this is a thread performing some core activity
            //     abort();
            // else if this is a thread used to service requests
            //    pthread_exit();
        }
    };
public:
    ExceptionHandler()
    {
        static SingleTonHandler s_objHandler;
    }
};        

#endif	/* SYSEXCEPTION_H */

