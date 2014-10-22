#include "schema.h"

#include "../lib_common/log.h"
#include "../lib_common/file.h"
#include "../lib_common/directory.h"


#include <sstream>
#include <assert.h>
#include <bits/stl_map.h>
#include <vector>

Schema* Schema::instance = NULL;

Schema& Schema::get_instance()
{
    if (instance == NULL) {
        instance = new Schema();
    } 
    
    return (*instance);
}

bool Schema::load() 
{
    string currentPath = "";
    Directory::get_current_path(currentPath); 
    
    string schema_abs_path = currentPath + schema_file;
    if (false == File::exist(schema_abs_path)) {
        Log::get_instance().log().error("load schema file, schema file not exist. file path:%s.", schema_abs_path.c_str()); 
        return false;
    }
    
    TiXmlDocument doc_patterns(schema_abs_path.c_str());  
    if(false == doc_patterns.LoadFile())
    {
        Log::get_instance().log().error("load schema file, load file failure. file:%s, error info:%s.", 
                                         schema_abs_path.c_str(),
                                         doc_patterns.ErrorDesc()); 
        return false;
    }
    
    TiXmlElement *root_element = doc_patterns.RootElement();  
    if(root_element == NULL)
    {
        Log::get_instance().log().error("get root element failure."); 
        return false;
    }
    
    if(false == load_schema(root_element))
    {
        Log::get_instance().log().error("load servers element failure."); 
        return false;
    }     
  
    return true;
}

bool Schema::load_schema(TiXmlElement *root_node)
{
    if(NULL == root_node)
    {
        Log::get_instance().log().error("schemas element is NULL."); 
        return false;
    }
    
    TiXmlElement *schema_element = root_node->FirstChildElement("Schema");
    while(NULL != schema_element)
    {       
        SchemaServer server;        
        
        if(false == load_server(schema_element, server))
        {
             Log::get_instance().log().error("load server element failure."); 
             return false;
        }     
        
        if(false == load_databases(schema_element, server))
        {
             Log::get_instance().log().error("load databases element failure."); 
             return false;
        }     
        
        schema_server_sets.insert(std::pair<int, SchemaServer>(server.id, server));     
        
        schema_element = schema_element->NextSiblingElement("Schema");
    }
    
    return true;
}

bool Schema::load_server(TiXmlElement *schema_node, SchemaServer &server)
{
    if(NULL == schema_node)
    {
        Log::get_instance().log().error("schema element is NULL."); 
        return false;
    }
    
    TiXmlElement *server_element = schema_node->FirstChildElement("Server");
    if(NULL == server_element)
    {
        Log::get_instance().log().error("server element is NULL."); 
        return false;
    }
        
    TiXmlElement *server_id_element = server_element->FirstChildElement("ID");
    if(NULL == server_id_element)
    {
        Log::get_instance().log().error("server id element is NULL."); 
        return false;
    }
        
    server.id = atoi(server_id_element->FirstChild()->Value());
     
    return true;
}

bool Schema::load_databases(TiXmlElement *schema_node, SchemaServer &server)
{
    if(NULL == schema_node)
    {
        Log::get_instance().log().error("schema element is NULL."); 
        return false;
    }
    
    TiXmlElement *databases_element = schema_node->FirstChildElement("Databases");
    if(NULL == databases_element)
    {
        Log::get_instance().log().error("databases element is NULL."); 
        return false;
    }
    
    TiXmlElement *database_element = schema_node->FirstChildElement("Database");   
    while(NULL != database_element)
    {       
        SchemaDatabase database;                
        if(false == load_database(database_element, database))
        {
             Log::get_instance().log().error("load databases element failure."); 
             return false;
        }     
        
        server.databases.insert(std::pair<string, SchemaDatabase>(database.name, database));     
        
        database_element = database_element->NextSiblingElement("Database");
    }
    
    return true;
}

bool Schema::load_database(TiXmlElement *database_node, SchemaDatabase &database)
{
    if(NULL == database_node)
    {
        Log::get_instance().log().error("database element is NULL."); 
        return false;
    }
    
    TiXmlElement *database_name_element = database_node->FirstChildElement("Name");
    if(NULL == database_name_element)
    {
        Log::get_instance().log().error("database name element is NULL."); 
        return false;
    }
    database.name = database_name_element->FirstChild()->Value();
    
    TiXmlElement *tables_element = database_node->FirstChildElement("Tables");   
    if(NULL == tables_element)
    {
        Log::get_instance().log().error("tables element is NULL."); 
        return false;
    }
    
    TiXmlElement *table_element = tables_element->FirstChildElement("Table");   
    while(NULL != table_element)
    {       
        SchemaTable table;                
        if(false == load_table(table_element, table))
        {
             Log::get_instance().log().error("load table element failure."); 
             return false;
        }     
        
        database.tables.insert(std::pair<string, SchemaTable>(table.name, table));     
        
        table_element = table_element->NextSiblingElement("Table");
    }
    
    return true;
}

bool Schema::load_table(TiXmlElement *table_node, SchemaTable &table)
{
    if(NULL == table_node)
    {
        Log::get_instance().log().error("table element is NULL."); 
        return false;
    }
    
    TiXmlElement *table_name_element = table_node->FirstChildElement("Name");
    if(NULL == table_name_element)
    {
        Log::get_instance().log().error("table name element is NULL."); 
        return false;
    }
    table.name = table_name_element->FirstChild()->Value();
    
    TiXmlElement *columns_element = table_node->FirstChildElement("Columns");   
    if(NULL == columns_element)
    {
        Log::get_instance().log().error("columns element is NULL."); 
        return false;
    }
    
    TiXmlElement *column_element = columns_element->FirstChildElement("Column");   
    while(NULL != column_element)
    {       
        SchemaColumn column;                
        if(false == load_column(column_element, column))
        {
             Log::get_instance().log().error("load column element failure."); 
             return false;
        }     
        
        table.columns.push_back(column);
        
        column_element = column_element->NextSiblingElement("Column");
    }
    
    return true;
}

bool Schema::load_column(TiXmlElement *column_node, SchemaColumn &column)
{
    if(NULL == column_node)
    {
        Log::get_instance().log().error("column element is NULL."); 
        return false;
    }
    
    TiXmlElement *column_name_element = column_node->FirstChildElement("Name");
    if(NULL == column_name_element)
    {
        Log::get_instance().log().error("column name element is NULL."); 
        return false;
    }
    
    column.name = column_name_element->FirstChild()->Value();
    return true;
}