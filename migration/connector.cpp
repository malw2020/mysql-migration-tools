#include "connector.h"
#include "sys_config.h"

#include <mysql.h>

MySqlConnector* MySqlConnector::m_instance = NULL;

MySqlConnector& MySqlConnector::getInstance()
{
    if (m_instance == NULL) {
        m_instance = new MySqlConnector();
        
        // init conn parameter 
        m_instance->m_destIP       = SysConfig::getInstance().m_destIP; 
        m_instance->m_destPost     = SysConfig::getInstance().m_destPost;
        m_instance->m_userName     = SysConfig::getInstance().m_userName;
        m_instance->m_userPassword = SysConfig::getInstance().m_userPassword;
        m_instance->m_destDatabase = SysConfig::getInstance().m_destDatabase;
        
        m_instance->connect();
    } 
    
    return (*m_instance);
}

bool MySqlConnector::replicate(string sqlCmd) {
    if (isConnected == false || m_mysql == NULL)
        return false;
        
    MYSQL_RES *result = NULL;
    unsigned int num_fields = 0, num_rows = 0;
    if (mysql_query(m_mysql, sqlCmd.c_str())) {
        fprintf(stderr, "Failed to query, sql cmd:%s, Error: %s\n", sqlCmd.c_str(), mysql_error(m_mysql));
    }   
    else { 
        // query succeeded, process any data returned by it
        result = mysql_store_result(m_mysql);
        if (result) { 
            num_fields = mysql_num_fields(result);
            mysql_free_result(result);
            return true;
        }
        else { 
            // mysql_store_result() returned nothing
            if (mysql_field_count(m_mysql) == 0) {
                // query does not return data (it was not a SELECT)
                num_rows = mysql_affected_rows(m_mysql);
                return true;
            }
            else { 
                // mysql_store_result() should have returned data
                fprintf(stderr, "Failed to query, sql cmd:%s, Error: %s\n", sqlCmd.c_str(), mysql_error(m_mysql));
            }
        }
    }
              
    return false;
}

bool MySqlConnector::connect() {
    if (isConnected == true && m_mysql != NULL)
        return true;
      
    isConnected = false;
    m_mysql = mysql_init(NULL);

    if (!m_mysql)
        return false;
          
    if (!mysql_real_connect(m_mysql, m_destIP.c_str(), m_userName.c_str(), m_userPassword.c_str(), 
                            m_destDatabase.c_str(), m_destPost, 0, 0)) {
        mysql_close(m_mysql);
        m_mysql = NULL;
        fprintf(stderr, "Failed to connect to database, host:%s, user:%s, pass:%s, db:%s, port:%d, Error: %s\n",
                         m_destIP.c_str(), m_userName.c_str(), m_userPassword.c_str(), 
                         m_destDatabase.c_str(), m_destPost, mysql_error(m_mysql));
        return false;
    }
    else {
        isConnected = true;
    }
    
    return true;
}

bool MySqlConnector::disconnect() {
    isConnected = false;
    mysql_close(m_mysql);
    m_mysql = NULL;
    return true;
}