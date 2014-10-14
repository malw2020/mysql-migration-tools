#include "exception_stack_trace.h"

ExceptionStackTrace* ExceptionStackTrace::get_instance()
{
    if (NULL == _instance)
    {
        _instance = new ExceptionStackTrace();
    }
    
    return _instance;
}

void ExceptionStackTrace::output(FILE *out, unsigned int max_frames)
{
    fprintf(out, "Stack Trace:\n");

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
         fprintf(out, "  <empty, possibly corrupt>\n");
         return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()
    char** symbollist = backtrace_symbols(addrlist, addrlen);
    if(symbollist == NULL)
        return;

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 1024;
    char* funcname = (char*)malloc(funcnamesize);
    string fileName, beginName, offset;

    // iterate over the returned symbol lines. skip the first, it is the address of this function.
    for (int i = 1; i < addrlen; i++) {
        if (true == splitMessage(symbollist[i], fileName, beginName, offset)) {
            // buile command
            string cmd = buildCommand(symbollist[i]);
            // get file and line infomation
                string fileinfo = getFileAndLineInfo(cmd);

                int status;
                char* ret = abi::__cxa_demangle(beginName.c_str(), funcname, &funcnamesize, &status);

                if (0 == status) {
                    funcname = ret; // use possibly realloc()-ed string
                    fprintf(out, "%s : %s+%s [%s]\n", fileName.c_str(), funcname, offset.c_str(), fileinfo.c_str());
                }
                else {
                    // demangling failed. Output function name as a C function with no arguments.
                    fprintf(out, "%s : %s()+%s [%s]\n", fileName.c_str(), beginName.c_str(), offset.c_str(), fileinfo.c_str());
                }
            }
            else {
                // couldn't parse the line? print the whole line.
                fprintf(out, "%s\n", symbollist[i]);
            }
        }

        free(funcname);

        if(NULL != symbollist){
            free(symbollist);
            symbollist = NULL;
        }
}

bool ExceptionStackTrace::splitMessage(const char * rawInfomation, string &fileName, string &beginName, string &offset)
{
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;
        char buffer[1024] = {'\0'};
        strcpy(buffer, rawInfomation);
        
        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = buffer; *p; ++p) {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset  && begin_name < begin_offset) {
            *begin_name++   = '\0';
            *begin_offset++ = '\0';
            *end_offset     = '\0';

            fileName  = buffer;
            beginName = begin_name;
            offset    = begin_offset;

            return true;
        }

        return false;
    }

string ExceptionStackTrace::buildCommand(char * rawInfomation)
{
        char cmd[1024] = "addr2line -e ";
        char* prog = cmd + strlen(cmd);
        int len = readlink("/proc/self/exe" , prog , sizeof(cmd) - strlen(cmd) - 1);
        if(len <= 0 || len >= (int)sizeof(cmd))
        {
            return string("");
        }

        bool startPos = false;
        for (char *pos = rawInfomation; *pos; ++pos) {
            if ('[' == *pos) {
                startPos = true;
                len = strlen(cmd);
                cmd[len]= ' ';
                cmd[len + 1] = '\0';
            }
            else if (']' == *pos) {
                break;
            }
            else {
                if(startPos == true) {
                    len = strlen(cmd);
                    cmd[len]= *pos;
                    cmd[len + 1] = '\0';
                }
            }
        }

        return string(cmd);
}

string ExceptionStackTrace::getFileAndLineInfo(string cmd)
{
    char infomation[1024] = {'\0'};
    FILE* fp = popen(cmd.c_str(), "r");
    if (NULL == fp)
        return string("");

    while(fgets(infomation, sizeof(infomation), fp) != NULL){
        if('\n' == infomation[strlen(infomation)-1]) {
             infomation[strlen(infomation)-1] = '\0';
        }
    }

    pclose(fp);

    return string(infomation);
}

ExceptionStackTrace* ExceptionStackTrace::_instance = NULL;
