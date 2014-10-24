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

#include "main.h"
#include "mysql_replication.h"
#include "../lib_common/log.h"
#include "../lib_meta/schema.h"

using namespace std;

void save_replication_state()
{
    if(true == ReplicationState::get_instance().save_replication_info())
        Log::get_instance().log().info("save replication state info successful.");
    else
        Log::get_instance().log().info("save replication state info failure.");
}

bool sys_init()
{
    // load schema info
    if(false == Schema::get_instance().load())
    {
        Log::get_instance().log().error("init schema failure.");
        return false;
    }
    Log::get_instance().log().info("init schema successful.");
    
    // load ReplicationState info
    if(false == ReplicationState::get_instance().init_relication_info()) 
    {
        Log::get_instance().log().error("init replication state info failure.");
        return false;
    }
    Log::get_instance().log().info("init replication state info successful.");
    
    // load Replication Patterns info
    if(false == ReplicationPatterns::get_instance().load()) 
    {
        Log::get_instance().log().error("init replication patterns info failure.");
        return false;
    }
    Log::get_instance().log().info("init replication patterns info successful.");
    
    // init Dispatcher and connect mysql database
    if(false == Dispatcher::get_instance().load())
    {
        Log::get_instance().log().error("init dispatcher and connect mysql database failure.");
        return false;
    }
    Log::get_instance().log().info("init dispatcher and connect mysql database successful.");
    
    // set exception hook
    g_objSegmentationFaultTranslator.set(save_replication_state);
    g_objFloatingPointExceptionTranslator.set(save_replication_state);
    g_objSigINTExceptionTranslator.set(save_replication_state);
    
    return true;
}

//Usage: mysqlreplication mysql://dddd:dddd@192.168.1.197:3306
int main(int argc, char** argv) 
{
    Log::get_instance().log().info("mysql-migration-tool start...");
        
    // init system
    if(false == sys_init())
        return -1;
    
    // concat command string
    SourceNode& source_node = ReplicationPatterns::get_instance().get_source_node();
    string source_driver    = ReplicationPatterns::get_instance().get_command_line(source_node);
    Binary_log binlog(create_transport(source_driver.c_str()));
    Log::get_instance().log().info("source driver command line:%s", source_driver.c_str());

    // bind query process
    QueryVariables query_var(source_node);
    binlog.content_handler_pipeline()->push_back(&query_var);
    
    // bind rotate process, change binlog file and position
    RotateVariables rotate_var(source_node);
    binlog.content_handler_pipeline()->push_back(&rotate_var);
    
    // support row replication
    ServerTableMap server_table_map;
    TableMapVariables table_map_var(source_node, &server_table_map);
    binlog.content_handler_pipeline()->push_back(&table_map_var);
    
    RowVariables row_var(source_node, &server_table_map);
    binlog.content_handler_pipeline()->push_back(&row_var);

    int result =  binlog.connect(source_node.bin_log_file, source_node.position);
    if(ERR_OK != result)
    {
        Log::get_instance().log().error("connect to master failure.");
        return -1;
    }
    
    ulong currrent_pos = 0;    
    while (true) 
    {
        Binary_log_event *event = NULL;
        result = binlog.wait_for_next_event(&event);
        if (result != ERR_OK)
        {
            Log::get_instance().log().info("wait for next event error, error code: %d", result);
            break;
        }
        else
        {
        }
               
        if(event == NULL)
        {
            break;
        }
            
        Log::get_instance().log().info("event type: %d", static_cast<int>(event->get_event_type()));
        switch (event->get_event_type()) 
        {
            case QUERY_EVENT:
                break;
            case ROTATE_EVENT:
                break;
            case FORMAT_DESCRIPTION_EVENT:
                break;
            default:
                break;
        }
        
        currrent_pos = binlog.get_position();
        rotate_var.update_binlog_pos(currrent_pos);
        
        if(event != NULL)
            delete event;
    }
    
    binlog.disconnect();
    
    Log::get_instance().log().info("mysql-migration-tool end...");
}   
