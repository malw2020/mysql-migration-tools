/* 
 * File:   main.h
 * Author: maliwei
 *
 * Created on 2014年10月21日, 21:53
 */

#ifndef MAIN_H
#define	MAIN_H

#include "../lib_common/sys_exception.h"

SignalTranslator<SegmentationFaultException>  g_objSegmentationFaultTranslator;
SignalTranslator<FloatingPointFaultException> g_objFloatingPointExceptionTranslator;
SignalTranslator<SigINTException>             g_objSigINTExceptionTranslator;

ExceptionHandler g_objExceptionHandler;


#endif	/* MAIN_H */

