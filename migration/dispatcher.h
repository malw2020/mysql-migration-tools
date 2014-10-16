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
        unload();
    }
    
public:
    static Dispatcher& get_instance();
    bool replicate(MasterInfo &master, string sql_cmd);

    bool load();
    bool reload();
    
    bool unload();
    
private:
    int get_one_to_one_rand_seed();
    int get_one_to_many_rand_seed(unsigned int max);   
    bool load_ono_to_one();
    bool load_one_to_many();
    
private:
    static Dispatcher* instance;
    
private:
    std::map<MasterInfo, MySQLAdapterSets*> master_mysql_adapters;
};

#endif	/* DISPATCHER_H__ */

