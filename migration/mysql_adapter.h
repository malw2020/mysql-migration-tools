/* 
 * File:   mysql_adapter.h
 * Author: maliwei
 *
 * Created on 2014年10月15日, 22:42
 */

#ifndef MYSQL_ADAPTER_H
#define	MYSQL_ADAPTER_H

#include <my_global.h>
#include <mysql.h>
#include <m_ctype.h>
#include <sql_common.h>

#ifdef min //definition of min() and max() in std and libmysqlclient
           //can be/are different
#undef min
#endif

#ifdef max
#undef max
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class MySQLAdapter
{
public:
    MySQLAdapter(){
         handle_mysql       = NULL; 
         is_connected   = false;
         reconn_counter = 0; 
    }
    
    MySQLAdapter(string ip, int port, string name, string password, string database){
         handle_mysql       = NULL; 
         is_connected   = false;
         reconn_counter = 0; 
         
         dest_ip   = ip;
         dest_port = port;
         dest_user_name     = name;
         dest_user_password = password;
         dest_database      = database;
    }

    ~MySQLAdapter(){
        disconnect();
    }
    
public:
    bool query(string sql_cmd);
    
public:
    bool connect();   
    bool disconnect();
    
public:    
    string dest_ip;              // destination IP
    int    dest_port;            // destination Port
    string dest_user_name;       // destionation User
    string dest_user_password;   // destionation pawword
    string dest_database;        // destionation database
    
private:
    MYSQL *handle_mysql;
    bool is_connected;          // 
    int reconn_counter;         // try to connector, if reconn less than 10
};

typedef std::vector<MySQLAdapter> MySQLAdapterSets;

#endif	/* MYSQL_ADAPTER_H */

