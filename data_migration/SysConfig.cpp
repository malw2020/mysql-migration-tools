#include "SysConfig.h"
#include "file.h"
#include "iniFile.h"
#include "directory.h"

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