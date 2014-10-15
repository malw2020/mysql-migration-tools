/* 
 * File:   replication_patterns.h
 * Author: maliwei
 *
 */

#ifndef REPLICATION_PATTERNS_H__
#define	REPLICATION_PATTERNS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>

#include "global_macro.h"
#include "../lib_tinyxml/tinyxml.h"  
#include "../lib_tinyxml/tinystr.h" 

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
        
        error_description = "no error";
    }
public:
    ~ReplicationState(){
    }
    
public:
    static ReplicationState& get_instance();
    
    bool init_relication_info();
    bool update_replication_state(MasterInfo& master, ReplicationInfo& replication);
    bool save_replication_info();
    bool save_replication_info(MasterInfo& master, ReplicationInfo& replication);
    
private:
    void add_master_node(const MasterInfo& master, const ReplicationInfo& replication);
    string get_master_desc(const MasterInfo& master) const;
    
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

typedef struct tag_source_node
{
    string ip;
    int    port;
    string bin_log_file;
    uint64_t position;
    
    string user;
    string password;
    
    vector<string> replicate_do_db;
    vector<string> replicate_ignore_db;
} SourceNode;

typedef struct tag_destination_node
{
    string ip;
    int    port;
    string user;
    string password;
    string database;    
} DestinationNode;

class ReplicationPatterns
{
private:
    ReplicationPatterns(){        
        patterns_file = PATTERNS_FILE;
    }
    
public:
    ~ReplicationPatterns(){
    }
    
public:
    static ReplicationPatterns& get_instance();
    bool load();
    SourceNode& get_source_node();
    string get_command_line(SourceNode& source);
    
private:
    bool load_modes(TiXmlElement *mode_node);
    bool load_sources(TiXmlElement *sources_node);
    bool load_destinations(TiXmlElement *destinations_node);
    void add_master_node(const MasterInfo& master, const ReplicationInfo& replication);
    
public:
    int mode; //1 1->1; 2 1->n; 3; n->1; 4 n->n
    
    std::vector<SourceNode>      source_nodes;
    std::vector<DestinationNode> destination_nodes;  
    std::map<MasterInfo, ReplicationInfo> replications;
    
private:
    static ReplicationPatterns* instance;
    string patterns_file;
};

#endif	/* REPLICATION_PATTERNS_H__ */

