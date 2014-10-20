#include "mysql_adapter.h"
#include "../lib_common/log.h"

bool MySQLAdapter::query(string sql_cmd) {
    if (is_connected == false || handle_mysql == NULL)
        return false;
        
    MYSQL_RES *result = NULL;
    unsigned int num_fields = 0, num_rows = 0;
    if (mysql_query(handle_mysql, sql_cmd.c_str())) 
    {
        Log::get_instance().log().error("failed to query, sql cmd:%s, error desc: %s.", sql_cmd.c_str(), mysql_error(handle_mysql));
    }   
    else
    { 
        // query succeeded, process any data returned by it
        result = mysql_store_result(handle_mysql);
        if (NULL != result)
        { 
            num_fields = mysql_num_fields(result);
            mysql_free_result(result);
            return true; 
        }
        else 
        {              
            if(0 != mysql_errno(handle_mysql))
            {
                Log::get_instance().log().error("reading of the result set failed. sql cmd:%s, error code: %d, error desc: %s.", 
                        sql_cmd.c_str(), mysql_errno(handle_mysql), mysql_error(handle_mysql));   
                return false;
            }
            else if (mysql_field_count(handle_mysql) == 0) 
            {
                num_rows = mysql_affected_rows(handle_mysql);
                Log::get_instance().log().info("query OK, %d row affected.", num_rows);
                return true;     
            }
            else 
            { 
                // mysql_store_result() should have returned data
                Log::get_instance().log().error("unknown error, sql cmd:%s, error desc: %s.", sql_cmd.c_str(), mysql_error(handle_mysql));             
            }          
        }
    }
              
    return false;
}

bool MySQLAdapter::connect() {
    if (is_connected == true && handle_mysql != NULL)
        return true;
    
    handle_mysql = mysql_init(NULL);

    if (!handle_mysql)
    {
        Log::get_instance().log().error("mysql_init failure.");
        return false;
    }    
        
    if (!mysql_real_connect(handle_mysql, dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                            dest_database.c_str(), dest_port, 0, 0)) {
        mysql_close(handle_mysql);
        handle_mysql = NULL;
        Log::get_instance().log().error("connect to database failure, host:%s, user:%s, pass:%s, db:%s, port:%d, error desc: %s.",
                         dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                         dest_database.c_str(), dest_port, mysql_error(handle_mysql));
        return false;
    }
    else {
        is_connected = true;
    }
    
    Log::get_instance().log().info("connect to database successfull, host:%s, user:%s, pass:%s, db:%s, port:%d.",
                    dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                    dest_database.c_str(), dest_port);
    
    return true;
}

bool MySQLAdapter::disconnect() {
    is_connected = false;
    mysql_close(handle_mysql);
    handle_mysql = NULL;
    
     Log::get_instance().log().info("disconnect to database successfull, host:%s, user:%s, pass:%s, db:%s, port:%d.",
                    dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                    dest_database.c_str(), dest_port);
    return true;
}