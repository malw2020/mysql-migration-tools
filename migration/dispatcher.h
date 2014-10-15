/* 
 * File:   Connector.h
 * Author: maliwei
 *
 * Created on 2014-05-12, 23:45
 */

#ifndef DISPATCHER_H__
#define	DISPATCHER_H__

#include "replication_patterns.h"
#include "mysql_adapter.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <map>

using namespace std;

class Dispatcher
{
private:
    Dispatcher(){ 
    }
    
public:
    ~Dispatcher(){
    }
    
public:
    static Dispatcher& get_instance();
    bool replicate(MasterInfo &master, string sql_cmd);
       
private:
    static Dispatcher* instance;
    
private:
    std::map<MasterInfo, MySQLAdapterSets> master_mysql_adapters;
};

#endif	/* DISPATCHER_H__ */

