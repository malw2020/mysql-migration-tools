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
#include "SysException.h"
#include "SysConfig.h"
#include "iniFile.h"
#include "directory.h"
#include "Connector.h"

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


class MySqlToMySqlTransponder : public Content_handler {
public:
    MySqlToMySqlTransponder()
    {
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
};

class Rotate_variables : public Content_handler {
public:
    Rotate_variables()
    {
    }

    Binary_log_event *process_event(Rotate_event *ev) {
        if(ev == NULL)
            return NULL;
        
        m_binlog_file = ev->binlog_file;
        m_binlog_pos  = ev->binlog_pos;
        
        string currentPath = "";
        Directory::getCurrentPath(currentPath);  
    
        string cfgFile = currentPath + "replication.ini";          
        IniFile inifile(cfgFile);
        if (0 == inifile.write_profile_string("Replication", "BinLogFile", m_binlog_file)){
            printf("set replication binlog file name failure.\n");
            return NULL;
        }
    
        stringstream ss;
        ss<<m_binlog_pos;
        if (0 == inifile.write_profile_string("Replication", "Position", ss.str())){
            printf("set replication binlog position failure.\n");
            return NULL;
        }
            
        return ev;
    }
    
public:
    std::string m_binlog_file;
    uint64_t m_binlog_pos;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: mysqlreplication mysql://dddd:dddd@192.168.1.197:3306" << std::endl;
        return -1;
    }

    // load config information
    if(false == SysConfig::getInstance().load()) {
        return -1;
    }
    
    Binary_log binlog(create_transport(argv[1]));
    
    // bind query process
    MySqlToMySqlTransponder mysql2mysql;
    binlog.content_handler_pipeline()->push_back(&mysql2mysql);
    
    // bind rotate process, change binlog file and position
    Rotate_variables rotate_variable;
    binlog.content_handler_pipeline()->push_back(&rotate_variable);
    
    binlog.connect();    
    int result = binlog.set_position(SysConfig::getInstance().m_binLogFile, SysConfig::getInstance().m_position);
    if (ERR_OK != result )
    {
        std::cerr << "set bin log position failure." << std::endl;
        return -1;
    }
    
    int currrent_pos = 0;
    string currentPath = "";
    Directory::getCurrentPath(currentPath);  
    string cfgFile = currentPath + "replication.ini";          
    IniFile inifile(cfgFile);
    
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
                std::cout<<rotate_variable.m_binlog_file<<"  "<<rotate_variable.m_binlog_pos<<std::endl;
                break;
            case FORMAT_DESCRIPTION_EVENT:
                break;
            default:
                break;
        }
        
        currrent_pos = binlog.get_position();
        std::cout<<"current pos:"<<currrent_pos<<endl;
        
        stringstream ss;
        ss<<currrent_pos;
        if (0 == inifile.write_profile_string("Replication", "Position", ss.str())){
            printf("set replication binlog position failure.\n");
            return -1;
        }
        
        if(event != NULL)
            delete event;
    }
}   
