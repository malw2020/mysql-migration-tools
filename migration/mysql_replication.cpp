#include "mysql_replication.h"
#include "../lib_common/log.h"

#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <iostream>

using namespace std;

QueryVariables::QueryVariables(SourceNode& node)
{
    source_node = node;
    master_info.ip   = node.ip;
    master_info.port = node.port;
}

Binary_log_event* QueryVariables::process_event(Query_event *ev) 
{
    if(ev == NULL)
        return NULL;
        
//    if(strcmp(ev->query.c_str(), "BEGIN") == 0)
//        return ev;
    
    Log::get_instance().log().info("database:%s, query:%s", ev->db_name.c_str(), ev->query.c_str());
    if(validate_database(ev->db_name))
    {
        if (false == Dispatcher::get_instance().replicate(master_info, ev->query)) 
        {
            Log::get_instance().log().error("query failure, database:%s, query:%s.", ev->db_name.c_str(), ev->query.c_str());
            return NULL;
        } 
    }
                
    return ev;
}
    
bool QueryVariables::validate_database(string database)
{
    vector<string>::const_iterator result;
    result = find(source_node.replicate_ignore_db.begin(), source_node.replicate_ignore_db.end(), database);
    if(result != source_node.replicate_ignore_db.end())
        return false;
                    
    result = find(source_node.replicate_do_db.begin() , source_node.replicate_do_db.end(), database);
    if(result == source_node.replicate_do_db.end())
        return false;
    else
        return true;
}
    
RotateVariables::RotateVariables(SourceNode& node)
{
    source_node = node;
    master_info.ip   = node.ip;
    master_info.port = node.port;
    replication_info.bin_log_file = node.bin_log_file;
    replication_info.position     = node.position;
}

Binary_log_event* RotateVariables::process_event(Rotate_event *ev)
{
    if(ev == NULL)
        return NULL;

    replication_info.bin_log_file = ev->binlog_file;
    replication_info.position     = ev->binlog_pos;
        
    ReplicationState::get_instance().save_replication_info(master_info, replication_info);
                   
    return ev;
}
    
bool RotateVariables::update_binlog_pos(ulong pos)
{
    replication_info.position = pos;
    ReplicationState::get_instance().save_replication_info(master_info, replication_info);
    return true;
}

RowVariables::RowVariables(SourceNode& node)
{
    source_node = node;
    master_info.ip   = node.ip;
    master_info.port = node.port;
    replication_info.bin_log_file = node.bin_log_file;
    replication_info.position     = node.position;
}

Binary_log_event* RowVariables::process_event(Row_event *ev)
{
    if(ev == NULL)
        return NULL;

    ev->print_long_info(std::cout);
                   
    return ev;
}
   