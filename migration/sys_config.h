/* 
 * File:   SysConfig.h
 * Author: maliwei
 *
 */

#ifndef SYSCONFIG_H__
#define	SYSCONFIG_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <map>

#include "global_macro.h"

using namespace std;

typedef struct tag_master_info
{
    string ip;
    int    port;
    
    bool operator < (tag_master_info const& _A) const
    {

        if(ip.compare(_A.ip)<0) 
            return true;

        if(ip == _A.ip)
            return port < _A.port;

        return false;
    }
} MasterInfo;

typedef struct tag_repliction_info
{
    string   bin_log_file;
    uint64_t position;
} ReplicationInfo;

class ReplicationState
{
private:
    ReplicationState(){        
        patterns_file = PATTERNS_FILE;
        replication_state_file = REPLICATION_STATE_FILE;
    }
public:
    ~ReplicationState(){
    }
    
public:
    static ReplicationState& getInstance();
    
    bool init_relication_info();
    //bool update_replication_state(MasterInfo& master, ReplicationInfo& replication);
    //bool save_replication_info();
    
private:
    void add_master_node(MasterInfo& master, ReplicationInfo& replication);
    
public:
    string   error_ip;
    string   error_port;
    string   error_bin_log_file;
    uint64_t error_position;
    string   error_description;
    
private:
    map<MasterInfo, ReplicationInfo> replications;
    
private:
    static ReplicationState* instance;
    string patterns_file;
    string replication_state_file;
};

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

