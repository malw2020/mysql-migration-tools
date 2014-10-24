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

TableMapVariables::TableMapVariables(SourceNode& node, ServerTableMap* server_table)
{
    source_node = node;
    master_info.ip   = node.ip;
    master_info.port = node.port;
    replication_info.bin_log_file = node.bin_log_file;
    replication_info.position     = node.position;
    
    server_table_map = server_table;
}

Binary_log_event* TableMapVariables::process_event(Table_map_event *ev)
{
    if(NULL == ev)
        return NULL;
    
    if(NULL == server_table_map)
        return ev;
    
    ServerTableID server_table_id;
    server_table_id.server_id = ev->header()->server_id;
    server_table_id.table_id  = ev->table_id;
    
    DatabaseTableName database_table_name;
    database_table_name.database = ev->db_name;
    database_table_name.table    = ev->table_name;
    
    std::vector<uint8_t>::iterator it;
    for (it= ev->columns.begin(); it != ev->columns.end(); ++it)
    {
        database_table_name.column_type.push_back((int)*it);
    }
    
    for (it= ev->metadata.begin(); it != ev->metadata.end(); ++it)
    {
        database_table_name.metadata.push_back((int)*it);
    }

    for (it= ev->null_bits.begin(); it != ev->null_bits.end(); ++it)
    {
        database_table_name.null_bits.push_back((int)*it);
    }    
    if(false == Schema::get_instance().get_columns_name(server_table_id, database_table_name))
    {
        Log::get_instance().log().error("table map get column name info failure.");
        return ev;
    }
    
    (*server_table_map)[server_table_id] = database_table_name;

    ev->print_long_info(std::cout);
                   
    return ev;
}

RowVariables::RowVariables(SourceNode& node, ServerTableMap* server_table)
{
    source_node = node;
    master_info.ip   = node.ip;
    master_info.port = node.port;
    replication_info.bin_log_file = node.bin_log_file;
    replication_info.position     = node.position;
    
    server_table_map = server_table;
}

Binary_log_event* RowVariables::process_event(Row_event *ev)
{
    if(ev == NULL)
        return NULL;
    
    if(NULL == server_table_map)
    {
        Log::get_instance().log().warn("server table map is null.");
        return ev;
    }
    
    ServerTableID server_table_id;
    server_table_id.server_id = ev->header()->server_id;
    server_table_id.table_id  = ev->table_id;
    
    std::map<ServerTableID, DatabaseTableName>::iterator iterator_map;
    iterator_map = server_table_map->find(server_table_id);
    if(iterator_map == server_table_map->end())
    {
        Log::get_instance().log().warn("server table does not exist in server table map, server id:%d, table id: %d.",
                                       server_table_id.server_id, server_table_id.table_id); 
        return ev;
    }
    
    DatabaseTableName &databse_table = iterator_map->second;
    unsigned int len = databse_table.column_name.size();
    for(unsigned int index=0; index<len; ++index)
    {
        Log::get_instance().log().info("column name: %s", databse_table.column_name[index].name.c_str()); 
    }
    
    bool result = false;
    switch(ev->get_event_type())
    {
        case PRE_GA_WRITE_ROWS_EVENT:
        case WRITE_ROWS_EVENT_V1:
        case WRITE_ROWS_EVENT:
            Log::get_instance().log().info("Insert"); 
            result = write_row_event(ev, databse_table);
            break;            
        case PRE_GA_DELETE_ROWS_EVENT:
        case DELETE_ROWS_EVENT_V1:
        case DELETE_ROWS_EVENT:
            Log::get_instance().log().info("Delete");
            result = delete_row_event(ev, databse_table);
            break;
        case PRE_GA_UPDATE_ROWS_EVENT:
        case UPDATE_ROWS_EVENT_V1:
        case UPDATE_ROWS_EVENT:
            Log::get_instance().log().info("Update");
            result = update_row_event(ev, databse_table);
            break;
    }

    ev->print_long_info(std::cout);
    
    if (ev->flags == Row_event::STMT_END_F)
        erase_map(server_table_id);
                   
    return ev;
}

bool RowVariables::erase_map(ServerTableID &server_table_id)
{
    std::map<ServerTableID, DatabaseTableName>::iterator iterator_map;
    iterator_map = server_table_map->find(server_table_id);
    if(iterator_map == server_table_map->end())
    {
        Log::get_instance().log().warn("server table does not exist in server table map when erase, server id:%d, table id: %d.",
                                       server_table_id.server_id, server_table_id.table_id); 
        return false;
    }
    
    server_table_map->erase(iterator_map);
    Log::get_instance().log().info("remove server table map successful.");
    
    return true;
}

bool RowVariables::write_row_event(Row_event *ev, DatabaseTableName &database_table)
{
//### INSERT INTO `bianbian`.`Persons`
//### SET
//###   @1='uuu'
//###   @2='hello'
//###   @3=22
    
    string sql_cmd = "INSERT INTO `";
    sql_cmd.append(database_table.table);
    
    unsigned int col_total = database_table.column_name.size();
    unsigned int last_index = col_total - 1;
    for(unsigned int index=0; index<col_total; ++index)
    {
        if(index == 0)
        {
            sql_cmd.append(string("` (`") + database_table.column_name[index].name + string("`"));
            if(col_total == 1)
            {
                sql_cmd.append(") ");
            }
        }
        else if(index < last_index)
        {
            sql_cmd.append(string(", `") + database_table.column_name[index].name +  string("`"));
        }
        else
        {
            sql_cmd.append(string(", `") + database_table.column_name[index].name +string("`) "));
        }
    }
    
    sql_cmd.append("VALUES(");
    
    
    sql_cmd.append(")");
    
    Log::get_instance().log().info("row replication, sql cmd: %s.", sql_cmd.c_str());
    
    return true;
}
   
bool RowVariables::delete_row_event(Row_event *ev, DatabaseTableName &database_table)
{
//# DELETE FROM `bianbian`.`Persons`
//### WHERE
//###   @1='uuu'
//###   @2='hello'
//###   @3=22

    string sql_cmd = "DELETE FROM `";
    sql_cmd.append(database_table.table);
    
    unsigned int col_total = database_table.column_name.size();
    unsigned int last_index = col_total - 1;
    for(unsigned int index=0; index<col_total; ++index)
    {
        if(index == 0)
        {
            sql_cmd.append(string("` WHERE (`") + database_table.column_name[index].name + string("`=xx "));
            if(col_total == 1)
            {
                sql_cmd.append(") ");
            }
        }
        else if(index < last_index)
        {
            sql_cmd.append(string("AND `") + database_table.column_name[index].name +  string("`=xx "));
        }
        else
        {
            sql_cmd.append(string("AND `") + database_table.column_name[index].name +string("`=xx ) "));
        }
    }
       
    Log::get_instance().log().info("row replication, sql cmd: %s.", sql_cmd.c_str());
    
    return true;
}

bool RowVariables::update_row_event(Row_event *ev, DatabaseTableName &database_table)
{
//update Persons set `FirstName`='malw' where `Age`=21
//### UPDATE `bianbian`.`Persons`
//### WHERE
//###   @1='uuu'
//###   @2='hello'
//###   @3=21
//### SET
//###   @1='malw'
//###   @2='hello'
//###   @3=21

    string sql_cmd = "UPDATE `";
    sql_cmd.append(database_table.table);
    
    unsigned int col_total = database_table.column_name.size();
    unsigned int last_index = col_total - 1;
      for(unsigned int index=0; index<col_total; ++index)
    {
        if(index == 0)
        {
            sql_cmd.append(string("` SET (`") + database_table.column_name[index].name + string("`=xx "));
            if(col_total == 1)
            {
                sql_cmd.append(") ");
            }
        }
        else if(index < last_index)
        {
            sql_cmd.append(string(", `") + database_table.column_name[index].name +  string("`=xx "));
        }
        else
        {
            sql_cmd.append(string(", `") + database_table.column_name[index].name +string("`=xx ) "));
        }
    }
    
    for(unsigned int index=0; index<col_total; ++index)
    {
        if(index == 0)
        {
            sql_cmd.append(string("` WHERE (`") + database_table.column_name[index].name + string("`=xx "));
            if(col_total == 1)
            {
                sql_cmd.append(") ");
            }
        }
        else if(index < last_index)
        {
            sql_cmd.append(string("AND `") + database_table.column_name[index].name +  string("`=xx "));
        }
        else
        {
            sql_cmd.append(string("AND `") + database_table.column_name[index].name +string("`=xx ) "));
        }
    }
       
    Log::get_instance().log().info("row replication, sql cmd: %s.", sql_cmd.c_str());
    
    return true;
}
