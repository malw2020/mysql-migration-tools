/* 
 * File:   log.h
 * Author: maliwei
 *
 * Created on 2014年10月17日, 20:40
 */

#ifndef LOG_H__
#define	LOG_H__

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include <string>
#include <iostream>

using namespace std;
using namespace log4cpp;

class Log
{
private:
    Log(){        
        log_properties = "log4cpp.properties";
        
        try
	{
            log4cpp::PropertyConfigurator::configure(log_properties);
	}
	catch (log4cpp::ConfigureFailure e)
	{
	    log4cpp::Category::getRoot().warn(e.what());
            std::cerr << "configure fail:" << e.what() << std::endl;
	}
    }
public:
    ~Log(){
        log4cpp::Category::shutdown();
    }
    
public:
    static Log& get_instance();
    
public:
    log4cpp::Category& root();
    log4cpp::Category& log();
    log4cpp::Category& report();
    
private:
    static void update_log_file_name();
        
private:
    static Log* instance;
    string log_properties;
};


#endif	/* LOG_H__ */

