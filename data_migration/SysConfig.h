/* 
 * File:   SysConfig.h
 * Author: malw
 *
 */

#ifndef SYSCONFIG_H__
#define	SYSCONFIG_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <map>

using namespace std;

class SysConfig
{
private:
    SysConfig(){        
        m_cfgFile = "replication.ini";
    }
public:
    ~SysConfig(){
    }
    
public:
    static SysConfig& getInstance();
    bool load();
    
public:
    string m_binLogFile;       // source replication start file name
    uint64_t m_position;       // source replication start position
    string m_sourceDatabase;   // source database
    
    string m_destIP;           // destination IP
    int    m_destPost;         // destination Port
    string m_userName;         // destionation User
    string m_userPassword;     // destionation pawword
    string m_destDatabase;     // destionation database
        
private:
    static SysConfig* m_instance;
    string m_cfgFile;
};

#endif	/* SYSCONFIG_H__ */

