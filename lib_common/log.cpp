#include "log.h"
#include "directory.h"
#include "file.h"
#include "datetime.h"

Log* Log::instance = NULL;

Log& Log::get_instance()
{
    if (instance == NULL) {
        update_log_file_name();
        instance = new Log();
    } 
    
    return (*instance);
}

log4cpp::Category& Log::root()
{
    return log4cpp::Category::getRoot();
}
    
log4cpp::Category& Log::log()
{
    return log4cpp::Category::getInstance(std::string("Log"));
}
     
log4cpp::Category& Log::report()
{
    return log4cpp::Category::getInstance(std::string("Report"));
}

void Log::update_log_file_name() 
{
    string currentPath = "";
    Directory::get_current_path(currentPath);
    
    string datetime;
    Datetime::get_current_datatime(datetime);
    
    string logPath = currentPath + "log/";
    if (true == File::exist(logPath + "log.info")) {
        string newName = "log_" + datetime + ".info";
        if (false == File::rename(logPath, "log.info", newName)){
            printf("rename file name fail, file:log.info\n"); 
        }
    }
    
    string reportPath = currentPath + "log/";
    if (true == File::exist(reportPath + "report.info")) {
        string newName = "report_" + datetime + ".info";
       if (false == File::rename(reportPath, "report.info",  newName)){
           printf("rename file name fail, file:report.info\n"); 
       }
    }
}