#include "datetime.h"

bool Datetime::get_current_datatime(string &datetime)
{
    struct timeval tv;
    struct tm      tm;
    char buf[128];
    
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    
    sprintf(buf, "%04d%02d%02d%02d%02d%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    datetime = buf;
    
    return true;
}
