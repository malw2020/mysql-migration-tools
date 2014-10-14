/* 
 * File:   Connector.h
 * Author: maliwei
 *
 * Created on 2014-05-12, 23:45
 */

#ifndef CONNECTOR_H__
#define	CONNECTOR_H__

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
#include <map>

using namespace std;

class MySqlConnector
{
private:
    MySqlConnector(){
         m_mysql       = NULL; 
         isConnected   = false;
         reconnCounter = 0; 
    }
public:
    ~MySqlConnector(){
        disconnect();
    }
    
public:
    static MySqlConnector& getInstance();
    bool replicate(string sqlCmd);
    
private:
    bool connect();   
    bool disconnect();
    
public:    
    string m_destIP;           // destination IP
    int    m_destPost;         // destination Port
    string m_userName;         // destionation User
    string m_userPassword;     // destionation pawword
    string m_destDatabase;     // destionation database
    
private:
    MYSQL *m_mysql;
    bool isConnected;          // 
    int reconnCounter;         // try to connector, if reconn less than 10
    
private:
    static MySqlConnector* m_instance;
};

#endif	/* CONNECTOR_H__ */

