/* 
 * File:   mysql_replication.h
 * Author: maliwei
 *
 * Created on 2014年10月21日, 21:07
 */

#ifndef MYSQL_REPLICATION_H
#define	MYSQL_REPLICATION_H

#include "../lib_mysql_replication/binlog_api.h"
#include "../lib_meta/schema.h"
#include "replication_patterns.h"
#include "dispatcher.h"

using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::get_event_type_str;
using mysql::User_var_event;


class QueryVariables : public Content_handler {
public:
    QueryVariables(SourceNode& node);
    Binary_log_event *process_event(Query_event *ev);    
    bool validate_database(string database);
    
public:
    SourceNode source_node;
    MasterInfo master_info;
};

class RotateVariables : public Content_handler {
public:
    RotateVariables(SourceNode& node);
    Binary_log_event *process_event(Rotate_event *ev);
    bool update_binlog_pos(ulong pos);
    
public:
    SourceNode source_node;
    MasterInfo master_info;
    ReplicationInfo replication_info;
};

class TableMapVariables : public Content_handler {
public:
    TableMapVariables(SourceNode& node, ServerTableMap* server_table);
    Binary_log_event *process_event(Table_map_event *ev);
    
public:
    SourceNode source_node;
    MasterInfo master_info;
    ReplicationInfo replication_info;
    ServerTableMap* server_table_map;
};

class RowVariables : public Content_handler {
public:
    RowVariables(SourceNode& node, ServerTableMap* server_table);
    Binary_log_event *process_event(Row_event *ev);
    
private:
    bool erase_map(ServerTableID &server_table_id);
    bool write_row_event(Row_event *ev, DatabaseTableName &database_table);
    bool delete_row_event(Row_event *ev, DatabaseTableName &database_table);
    bool update_row_event(Row_event *ev, DatabaseTableName &database_table);
    
public:
    SourceNode source_node;
    MasterInfo master_info;
    ReplicationInfo replication_info;
    ServerTableMap* server_table_map;
};



//  WRITE_ROWS_EVENT_V1 = 23,
//  UPDATE_ROWS_EVENT_V1 = 24,
//  DELETE_ROWS_EVENT_V1 = 25,

#endif	/* MYSQL_REPLICATION_H */

