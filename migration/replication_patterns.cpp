#include "replication_patterns.h"
#include "../lib_common/log.h"
#include "../lib_common/file.h"
#include "../lib_common/directory.h"
#include "../lib_common/ini_file.h"
#include "../lib_tinyxml/tinyxml.h"  
#include "../lib_tinyxml/tinystr.h" 

#include <sstream>
#include <assert.h>

using namespace std;

ReplicationState* ReplicationState::instance = NULL;

ReplicationState& ReplicationState::get_instance()
{
    if (instance == NULL) {
        instance = new ReplicationState();
    } 
    
    return (*instance);
}

bool ReplicationState::init_relication_info() {
    string currentPath = "";
    Directory::get_current_path(currentPath); 
    
    string patterns_abs_path = currentPath + patterns_file;
    if (false == File::exist(patterns_abs_path)) {
        Log::get_instance().log().error("patterns file not exist. file path:%s.", patterns_abs_path.c_str()); 
        return false;
    }
    
    TiXmlDocument doc_patterns(patterns_abs_path.c_str());  
    if(false == doc_patterns.LoadFile())
    {
        Log::get_instance().log().error("load patterns file failure. file:%s, error info:%s.", 
                                         patterns_abs_path.c_str(),
                                         doc_patterns.ErrorDesc()); 
        return false;
    }
    
    TiXmlElement *root_element = doc_patterns.RootElement();  
    if(root_element == NULL)
    {
        Log::get_instance().log().error("get root element failure."); 
        return false;
    }
    
    TiXmlElement *sources_element = root_element->FirstChildElement("Sources");
    if(sources_element == NULL)
    {
        Log::get_instance().log().error("get sources element failure."); 
        return false;
    }
    
    TiXmlElement *source_element = sources_element->FirstChildElement("Source");
    while(NULL != source_element)
    {             
        TiXmlElement *ip_element   = source_element->FirstChildElement("IP");
        TiXmlElement *port_element = source_element->FirstChildElement("Port");
        
        MasterInfo master;  
        master.ip   = ip_element->FirstChild()->Value();
        master.port = atoi(port_element->FirstChild()->Value());
        
        TiXmlElement *file_element     = source_element->FirstChildElement("File");
        TiXmlElement *position_element = source_element->FirstChildElement("Position");
        
        ReplicationInfo replication;
        replication.bin_log_file = file_element->FirstChild()->Value();
        replication.position     = atol(position_element->FirstChild()->Value());
            
        // add master node info
        add_master_node(master, replication);
       
        source_element = source_element->NextSiblingElement("Source");
    }
    
    return true;
}

bool ReplicationState::update_replication_state(MasterInfo& master, ReplicationInfo& replication)
{
    std::map<MasterInfo, ReplicationInfo>::iterator iterator_replications;
    iterator_replications = replications.find(master);
    if(iterator_replications == replications.end())
        return false;

    replications[master] = replication;
    return true;
}

bool ReplicationState::save_replication_info()
{
    string currentPath = "";
    Directory::get_current_path(currentPath); 
    
    string replication_state_abs_path = currentPath + replication_state_file;
           
    IniFile inifile(replication_state_abs_path);
    if(0 == inifile.write_profile_string("ErrorInfo", "Description", error_description)){
        Log::get_instance().log().error("save replication info error desc failure.");
        return false;
    }
    
    if(0 == inifile.write_profile_string("ErrorInfo", "IP", error_ip)){
        Log::get_instance().log().error("save replication info error IP failure.");
        return false;
    }
    
    if(0 == inifile.write_profile_string("ErrorInfo", "Port", error_port)){
        Log::get_instance().log().error("save replication info error port failure.");
        return false;
    }
    
    if(0 == inifile.write_profile_string("ErrorInfo", "BinLogFile", error_bin_log_file)){
        Log::get_instance().log().error("save replication info error bin log file failure.");
        return false;
    }
    
    stringstream ss_error;
    ss_error<<error_position;
    if(0 == inifile.write_profile_string("ErrorInfo", "Position", ss_error.str())){
        Log::get_instance().log().error("save replication info error position failure.");
        return false;
    }
    
    if(0 == inifile.write_profile_string("ErrorInfo", "Description", error_description)){
        Log::get_instance().log().error("save replication info error desc failure.");
        return false;
    }
    
    std::map<MasterInfo, ReplicationInfo>::iterator iterator_replications;
    for(iterator_replications = replications.begin(); iterator_replications != replications.end(); ++iterator_replications)
    {
        string section = "Replication_" + get_master_desc(iterator_replications->first);
        if(0 == inifile.write_profile_string(section, "BinLogFile", iterator_replications->second.bin_log_file)){
            Log::get_instance().log().error("save replication info node file failure.");
            return false;
        }
        
        stringstream ss_node_pos;
        ss_node_pos<<iterator_replications->second.position;
        if(0 == inifile.write_profile_string(section, "Position", ss_node_pos.str())){
            Log::get_instance().log().error("save replication info node position failure.");
            return false;
        }
    }
    
    return true;
}

bool ReplicationState::save_replication_info(MasterInfo& master, ReplicationInfo& replication)
{
    if(false == update_replication_state(master, replication))
        return false;
    
    string currentPath = "";
    Directory::get_current_path(currentPath); 
    
    string replication_state_abs_path = currentPath + replication_state_file;
           
    IniFile inifile(replication_state_abs_path);
    if(0 == inifile.write_profile_string("ErrorInfo", "Description", error_description))
    {
        Log::get_instance().log().error("save replication info error desc failure.");
        return false;
    }
    
    string section = "Replication_" + get_master_desc(master);
    if(0 == inifile.write_profile_string(section, "BinLogFile", replication.bin_log_file))
    {
         Log::get_instance().log().error("save replication info node file failure.");
         return false;
    }
        
    stringstream ss_node_pos;
    ss_node_pos<<replication.position;
    if(0 == inifile.write_profile_string(section, "Position", ss_node_pos.str()))
    {
         Log::get_instance().log().error("save replication info node position failure.");
         return false;
    }
    
    return true;
}

void ReplicationState::add_master_node(const MasterInfo& master, const ReplicationInfo& replication)
{
     replications.insert(std::pair<MasterInfo, ReplicationInfo>(master, replication));
}

string ReplicationState::get_master_desc(const MasterInfo& master) const
{
    string node_desc = "";
    
    stringstream ss;
    ss<<master.port;
    node_desc = master.ip + "_" + ss.str();
    
    return node_desc;
}

ReplicationPatterns* ReplicationPatterns::instance = NULL;

ReplicationPatterns& ReplicationPatterns::get_instance()
{
    if (instance == NULL) {
        instance = new ReplicationPatterns();
    } 
    
    return (*instance);
}

bool ReplicationPatterns::load() 
{
    string currentPath = "";
    Directory::get_current_path(currentPath); 
    
    string patterns_abs_path = currentPath + patterns_file;
    if (false == File::exist(patterns_abs_path)) {
        Log::get_instance().log().error("load replication patterns, patterns file not exist. file path:%s.", patterns_abs_path.c_str()); 
        return false;
    }
    
    TiXmlDocument doc_patterns(patterns_abs_path.c_str());  
    if(false == doc_patterns.LoadFile())
    {
        Log::get_instance().log().error("load replication patterns, load file failure. file:%s, error info:%s.", 
                patterns_abs_path.c_str(),
                doc_patterns.ErrorDesc()); 
        return false;
    }
    
    TiXmlElement *root_element = doc_patterns.RootElement();  
    if(root_element == NULL)
    {
        Log::get_instance().log().error("get root element failure."); 
        return false;
    }
    
    TiXmlElement *mode_element = root_element->FirstChildElement("Mode");
    if(false == load_modes(mode_element))
    {
        Log::get_instance().log().error("load mode element failure."); 
        return false;
    }
    
    TiXmlElement *sources_element = root_element->FirstChildElement("Sources");
    if(false == load_sources(sources_element))
    {
        Log::get_instance().log().error("load sources element failure."); 
        return false;
    }
    
    TiXmlElement *destinations_element = root_element->FirstChildElement("Destinations");
    if(false == load_destinations(destinations_element))
    {
        Log::get_instance().log().error("load destinations element failure."); 
        return false;
    }
     
  
    return true;
}

bool ReplicationPatterns::load_modes(TiXmlElement *mode_node)
{
    if(NULL == mode_node)
    {
        Log::get_instance().log().error("mode element is NULL."); 
        return false;
    }
    
    string mode_type = mode_node->Attribute("type");  
    Log::get_instance().log().info("mode type = %s.", mode_type.c_str());
    
    mode = atoi(mode_type.c_str());
    return true;
}

bool ReplicationPatterns::load_sources(TiXmlElement *sources_node)
{
    if(NULL == sources_node)
    {
        Log::get_instance().log().error("sources element is NULL.\n"); 
        return false;
    }
    
    TiXmlElement *source_element = sources_node->FirstChildElement("Source");
    while(NULL != source_element)
    {       
        SourceNode source_node;
        TiXmlElement *ip_element   = source_element->FirstChildElement("IP");
        TiXmlElement *port_element = source_element->FirstChildElement("Port");
        
        MasterInfo master;  
        master.ip   = ip_element->FirstChild()->Value();
        master.port = atoi(port_element->FirstChild()->Value());
        source_node.ip   = master.ip;
        source_node.port = master.port;
        
        TiXmlElement *file_element     = source_element->FirstChildElement("File");
        TiXmlElement *position_element = source_element->FirstChildElement("Position");
        
        ReplicationInfo replication;
        replication.bin_log_file = file_element->FirstChild()->Value();
        replication.position     = atol(position_element->FirstChild()->Value());
        source_node.bin_log_file = replication.bin_log_file;
        source_node.position     = replication.position;
        
        TiXmlElement *user_element     = source_element->FirstChildElement("User");
        TiXmlElement *password_element = source_element->FirstChildElement("Password");
        source_node.user     = user_element->FirstChild()->Value();
        source_node.password = password_element->FirstChild()->Value();
        
        //Replicate_do_db
        TiXmlElement *do_db_element = source_element->FirstChildElement("Replicate_do_db");
        if(NULL != do_db_element)
        {
            TiXmlElement *database_element = do_db_element->FirstChildElement("Database");
            while(NULL != database_element)
            {
                string database_name = database_element->FirstChild()->Value();
                source_node.replicate_do_db.push_back(database_name);
                
                database_element = do_db_element->NextSiblingElement("Database");
            }            
        }
        
        // Replicate_ignore_db
        TiXmlElement *ignore_db_element = source_element->FirstChildElement("Replicate_ignore_db");
        if(NULL != do_db_element)
        {
            TiXmlElement *database_element = ignore_db_element->FirstChildElement("Database");
            while(NULL != database_element)
            {
                string database_name = database_element->FirstChild()->Value();
                source_node.replicate_do_db.push_back(database_name);
                
                database_element = ignore_db_element->NextSiblingElement("Database");
            }            
        }
            
        add_master_node(master, replication);
        source_nodes.push_back(source_node);        
        
        source_element = source_element->NextSiblingElement("Source");
    }
    
    return true;
}

bool ReplicationPatterns::load_destinations(TiXmlElement *destinations_node)
{
    if(NULL == destinations_node)
    {
        Log::get_instance().log().error("destinations element is NULL."); 
        return false;
    }
    
    TiXmlElement *destination_element = destinations_node->FirstChildElement("Destination");
    while(NULL != destination_element)
    {       
        DestinationNode dest_node;
        
        TiXmlElement *ip_element       = destination_element->FirstChildElement("IP");
        TiXmlElement *port_element     = destination_element->FirstChildElement("Port");
        TiXmlElement *user_element     = destination_element->FirstChildElement("User");
        TiXmlElement *password_element = destination_element->FirstChildElement("Password");
        TiXmlElement *database_element = destination_element->FirstChildElement("Database");
        
        dest_node.ip       = ip_element->FirstChild()->Value();
        dest_node.port     = atoi(port_element->FirstChild()->Value());
        dest_node.user     = user_element->FirstChild()->Value();
        dest_node.password = password_element->FirstChild()->Value();
        dest_node.database = database_element->FirstChild()->Value();
        
        destination_nodes.push_back(dest_node);        
        
        destination_element = destination_element->NextSiblingElement("Destination");
    }
    
    return true;
}

void ReplicationPatterns::add_master_node(const MasterInfo& master, const ReplicationInfo& replication)
{
     replications.insert(std::pair<MasterInfo, ReplicationInfo>(master, replication));
}

SourceNode& ReplicationPatterns::get_source_node()
{
    if(source_nodes.empty() == true)
    {
        assert(!"source node is not exist!");
    }
    
    return source_nodes[0];
}

string ReplicationPatterns::get_command_line(SourceNode& source)
{
    stringstream source_driver;
    source_driver<<"mysql://"<<source.user<<":"<<source.password<<"@"<<source.ip<<":"<<source.port;
    
    return source_driver.str();
}
