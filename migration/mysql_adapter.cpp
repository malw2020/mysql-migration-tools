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
    
    handle_mysql = mysql_init(NULL);

    if (!handle_mysql)
    {
        printf("mysql_init failure\n");
        return false;
    }    
    printf("mysql_init successful.\n");
    
    if (!mysql_real_connect(handle_mysql, dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                            dest_database.c_str(), dest_port, 0, 0)) {
        mysql_close(handle_mysql);
        handle_mysql = NULL;
        printf("connect to database failure, host:%s, user:%s, pass:%s, db:%s, port:%d, Error: %s\n",
                         dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                         dest_database.c_str(), dest_port, mysql_error(handle_mysql));
        return false;
    }
    else {
        is_connected = true;
    }
    
    printf("connect to database successfull, host:%s, user:%s, pass:%s, db:%s, port:%d.\n",
                    dest_ip.c_str(), dest_user_name.c_str(), dest_user_password.c_str(), 
                    dest_database.c_str(), dest_port);
    
    return true;
}

bool MySQLAdapter::disconnect() {
    is_connected = false;
    mysql_close(handle_mysql);
    handle_mysql = NULL;
    return true;
}