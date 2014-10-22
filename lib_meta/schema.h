/* 
 * File:   schema.h
 * Author: maliwei
 *
 * Created on 2014年10月22日, 22:22
 */

#ifndef SCHEMA_H__
#define	SCHEMA_H__

#include "../lib_tinyxml/tinyxml.h"  
#include "../lib_tinyxml/tinystr.h" 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

typedef struct tag_schema_column
{
    string   name;
} SchemaColumn;

typedef std::vector<SchemaColumn> SchemaColumnSets;

typedef struct tag_schema_table
{
    string           name;
    SchemaColumnSets columns;
} SchemaTable;

typedef std::vector<SchemaTable> SchemaTableSets;

typedef struct tag_schema_database
{
    string      name;
    map<string, SchemaTable> tables;
} SchemaDatabase;

typedef std::vector<SchemaDatabase> SchemaDatabaseSets;

typedef struct tag_schema_server
{
    int id;
    map<string, SchemaDatabase> databases;
} SchemaServer;

typedef std::map<int, SchemaServer> SchemaServerSets;

class Schema
{
private:
    Schema(){        
        schema_file = "schema.xml";
    }
    
public:
    ~Schema(){
    }
    
public:
    static Schema& get_instance();
    bool load();
    
private:
    bool load_schema(TiXmlElement *root_node);
    bool load_server(TiXmlElement *schema_node, SchemaServer &server);
    bool load_databases(TiXmlElement *schema_node, SchemaServer &server);
    bool load_database(TiXmlElement *database_node, SchemaDatabase &database);
    bool load_table(TiXmlElement *table_node, SchemaTable &table);
    bool load_column(TiXmlElement *column_node, SchemaColumn &column);
    
public:
    SchemaServerSets schema_server_sets;
    
private:
    static Schema* instance;
    string schema_file;
};

#endif	/* SCHEMA_H__ */

