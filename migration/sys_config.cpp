#include "sys_config.h"
#include "../lib_common/file.h"
#include "../lib_common/ini_file.h"
#include "../lib_common/directory.h"
#include "../lib_tinyxml/tinyxml.h"  
#include "../lib_tinyxml/tinystr.h" 

ReplicationState* ReplicationState::instance = NULL;

ReplicationState& ReplicationState::getInstance()
{
    if (instance == NULL) {
        instance = new ReplicationState();
    } 
    
    return (*instance);
}

bool ReplicationState::init_relication_info() {
    string currentPath = "";
    Directory::getCurrentPath(currentPath); 
    
    string patterns_abs_path = currentPath + patterns_file;
    if (false == File::exist(patterns_abs_path)) {
        printf("patterns file not exist. file path:%s.\n", patterns_abs_path.c_str()); 
        return false;
    }
    
    TiXmlDocument doc_patterns(patterns_abs_path.c_str());  
    if(false == doc_patterns.LoadFile())
    {
        printf("load patterns file failure. file:%s, error info:%s.\n", 
                patterns_abs_path.c_str(),
                doc_patterns.ErrorDesc()); 
        return false;
    }
    
    TiXmlElement *root_element = doc_patterns.RootElement();  
    if(root_element == NULL)
    {
        printf("get root element failure. \n"); 
        return false;
    }
    
    TiXmlElement *sources_element = root_element->FirstChildElement("Sources");
    if(sources_element == NULL)
    {
        printf("get sources element failure.\n"); 
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

void ReplicationState::add_master_node(MasterInfo& master, ReplicationInfo& replication)
{
     printf("master node info -- mater info: %s, %d; replication info: %s, %d \n", 
             master.ip.c_str(), master.port,
             replication.bin_log_file.c_str(), replication.position);
     
     replications.insert(std::pair<MasterInfo, ReplicationInfo>(master, replication));
     
     printf("replications size = %d.\n", replications.size());
}

SysConfig* SysConfig::m_instance = NULL;

SysConfig& SysConfig::getInstance()
{
    if (m_instance == NULL) {
        m_instance = new SysConfig();
    } 
    
    return (*m_instance);
}

bool SysConfig::load() {
    string currentPath = "";
    Directory::getCurrentPath(currentPath);  
    
    string cfgFile = currentPath + m_cfgFile;
    if (false == File::exist(cfgFile)) {
        printf("config file not exist.\n"); 
        return false;
    }
           
    IniFile inifile(cfgFile);
    if(!inifile.read_profile_string("Source", "File", m_binLogFile, "")){
        printf("get binlog file name failure.\n");
        return false;
    }
    
    m_position = inifile.read_profile_int("Source", "Position", 0);
    if(m_position == 0){
        printf("get binlog position failure.\n");
        return false;
    }
    
    if(!inifile.read_profile_string("Source", "Database", m_sourceDatabase, "")){
        printf("get database name failure.\n");
        return false;
    }
    
    if(!inifile.read_profile_string("Destination", "IP", m_destIP, "")){
        printf("get destination IP failure.\n");
        return false;
    }
    
    m_destPost = inifile.read_profile_int("Destination", "Port", 0);
    if(m_destPost == 0){
        printf("get destionation port failure.\n");
        return false;
    }
    
    if(!inifile.read_profile_string("Destination", "User", m_userName, "")){
        printf("get destination user name failure.\n");
        return false;
    }
    
    if(!inifile.read_profile_string("Destination", "Password", m_userPassword, "")){
        printf("get destination user password failure.\n");
        return false;
    }
    
   if(!inifile.read_profile_string("Destination", "Database", m_destDatabase, "")){
        printf("get destination database password failure.\n");
        return false;
    }
    
    return true;
}