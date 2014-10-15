/*
Copyright (c) 2013, Oracle and/or its affiliates. All rights
reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of
the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
02110-1301  USA
*/

#include "../lib_mysql_replication/binlog_api.h"
#include "../lib_common/sys_exception.h"
#include "../lib_common/ini_file.h"
#include "../lib_common/directory.h"
#include "replication_patterns.h"
#include "connector.h"

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <string.h>
#include <algorithm>

using namespace std;

using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::get_event_type_str;
using mysql::User_var_event;

SignalTranslator<SegmentationFaultException> g_objSegmentationFaultTranslator;
SignalTranslator<FloatingPointFaultException> g_objFloatingPointExceptionTranslator;
ExceptionHandler g_objExceptionHandler;


class QueryVariables : public Content_handler {
public:
    QueryVariables(SourceNode& node)
    {
        source_node = node;
        master_info.ip   = node.ip;
        master_info.port = node.port;
    }

    Binary_log_event *process_event(Query_event *ev) {
        if(ev == NULL)
            return NULL;
        
        if(strcmp(ev->query.c_str(), "BEGIN") == 0)
            return ev;
        
        std::cout<<ev->db_name <<"  "<<ev->query<<std::endl;
        if(ev->db_name == SysConfig::getInstance().m_sourceDatabase) {
            if (false == MySqlConnector::getInstance().replicate(ev->query)) {
                printf("replication data to mysql failure.\n");
                return NULL;
            }
        }
        
        return ev;
    }
    
public:
    SourceNode source_node;
    MasterInfo master_info;
};

class RotateVariables : public Content_handler {
public:
    RotateVariables(SourceNode& node)
    {
        source_node = node;
        master_info.ip   = node.ip;
        master_info.port = node.port;
        replication_info.bin_log_file = node.bin_log_file;
        replication_info.position     = node.position;
    }

    Binary_log_event *process_event(Rotate_event *ev) {
        if(ev == NULL)
            return NULL;

        replication_info.bin_log_file = ev->binlog_file;
        replication_info.position     = ev->binlog_pos;
        
        ReplicationState::getInstance().save_replication_info(master_info, replication_info);
                   
        return ev;
    }
    
    bool update_binlog_pos(ulong pos)
    {
        replication_info.position = pos;
        ReplicationState::getInstance().save_replication_info(master_info, replication_info);
        return true;
    }
    
public:
    SourceNode source_node;
    MasterInfo master_info;
    ReplicationInfo replication_info;
};


//Usage: mysqlreplication mysql://dddd:dddd@192.168.1.197:3306
int main(int argc, char** argv) {
    // load ReplicationState info
    if(false == ReplicationState::getInstance().init_relication_info()) {
        std::cerr << "init relication state info failure." << std::endl;
        return -1;
    }
    
    // load Replication Patterns info
    if(false == ReplicationPatterns::getInstance().load()) {
        std::cerr << "init relication patterns info failure." << std::endl;
        return -1;
    }
    
    // concat command string
    SourceNode& source_node = ReplicationPatterns::getInstance().get_source_node();
    string source_driver    = ReplicationPatterns::getInstance().get_command_line(source_node);
    std::cout << "sourcer driver command line:" << source_driver.c_str() << std::endl;
    Binary_log binlog(create_transport(source_driver.c_str()));
    
    // bind query process
    QueryVariables query_var(source_node);
    binlog.content_handler_pipeline()->push_back(&query_var);
    
    // bind rotate process, change binlog file and position
    RotateVariables rotate_var(source_node);
    binlog.content_handler_pipeline()->push_back(&rotate_var);
    
    int result =  binlog.connect();
    if(ERR_OK != result)
    {
        std::cerr << "connect to master failure." << std::endl;
        return -1;
    }
    
    result = binlog.set_position(source_node.bin_log_file, source_node.position);
    if (ERR_OK != result )
    {
        std::cerr << "set bin log position failure." << std::endl;
        return -1;
    }
    
    ulong currrent_pos = 0;
       
    while (true) {
        Binary_log_event *event = NULL;
        result = binlog.wait_for_next_event(&event);
        if (result != ERR_OK)
            break;
               
        if(event == NULL)
        {
            break;
        }
            
        cout<<"evnet type:"<<static_cast<int>(event->get_event_type())<<endl;
        switch (event->get_event_type()) {
            case QUERY_EVENT:
                break;
            case ROTATE_EVENT:
                std::cout<<rotate_var.replication_info.bin_log_file<<"  "<<rotate_var.replication_info.position<<std::endl;
                break;
            case FORMAT_DESCRIPTION_EVENT:
                break;
            default:
                break;
        }
        
        currrent_pos = binlog.get_position();
        std::cout<<"current pos:"<<currrent_pos<<endl;
        rotate_var.update_binlog_pos(currrent_pos);
        
        if(event != NULL)
            delete event;
    }
}   
