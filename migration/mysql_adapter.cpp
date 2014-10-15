#include "mysql_adapter.h"

bool MySQLAdapter::query(string sql_cmd) {
    if (is_connected == false || handle_mysql == NULL)
        return false;
        
    MYSQL_RES *result = NULL;
    unsigned int num_fields = 0, num_rows = 0;
    if (mysql_query(handle_mysql, sql_cmd.c_str())) {
        fprintf(stderr, "Failed to query, sql cmd:%s, Error: %s\n", sql_cmd.c_str(), mysql_error(handle_mysql));
    }   
    else { 
        // query succeeded, process any data returned by it
        result = mysql_store_result(handle_mysql);
        if (result) { 
            num_fields = mysql_num_fields(result);
            mysql_free_result(result);
            return true;
        }
        else { 
            // mysql_store_result() returned nothing
            if (mysql_field_count(handle_mysql) == 0) {
                // query does not return data (it was not a SELECT)
                num_rows = mysql_affected_rows(handle_mysql);
                return true;
            }
            else { 
                // mysql_store_result() should have returned data
                fprintf(stderr, "Failed to query, sql cmd:%s, Error: %s\n", sql_cmd.c_str(), mysql_error(handle_mysql));
            }
        }
    }
              
    return false;
}

bool MySQLAdapter::connect() {
    if (is_connected == true && handle_mysql != NULL)
        return true;
      
    is_connected = false;
    handle_mysql = mysql_init(NULL);

    if (!handle_mysql)
        return false;
          
    if (!mysql_real_connect(handle_mysql, dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                            dest_database.c_str(), dest_port, 0, 0)) {
        mysql_close(handle_mysql);
        handle_mysql = NULL;
        fprintf(stderr, "Failed to connect to database, host:%s, user:%s, pass:%s, db:%s, port:%d, Error: %s\n",
                         dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                         dest_database.c_str(), dest_port, mysql_error(handle_mysql));
        return false;
    }
    else {
        is_connected = true;
    }
    
    return true;
}

bool MySQLAdapter::disconnect() {
    is_connected = false;
    mysql_close(handle_mysql);
    handle_mysql = NULL;
    return true;
}