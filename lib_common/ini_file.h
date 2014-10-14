/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 * @author Deng Yangjun
 * @date 2007-1-9
 * @version 0.2
 */
 
#ifndef INI_FILE_H_
#define INI_FILE_H_

#include <string>

using namespace std;

class IniFile{
public:
    IniFile(string path);
    ~IniFile();
    
    /**
    *@brief read string in initialization file\n
    * retrieves a string from the specified section in an initialization file
    *@param section [in] name of the section containing the key name
    *@param key [in] name of the key pairs to value 
    *@param value [in] pointer to the buffer that receives the retrieved string
    *@param size [in] size of result's buffer 
    *@param default_value [in] default value of result
    *@param file [in] path of the initialization file
    *@return 1 : read success; \n 0 : read fail
    */
    int read_profile_string(const string section, const string key, string &value, const string default_value = "");
    int read_profile_string(const char *section, const char *key, char *value, int size,const char *default_value);
    
    /**
    *@brief read int value in initialization file\n
    * retrieves int value from the specified section in an initialization file
    *@param section [in] name of the section containing the key name
    *@param key [in] name of the key pairs to value 
    *@param default_value [in] default value of result
    *@param file [in] path of the initialization file
    *@return profile int value,if read fail, return default value
    */
    int read_profile_int(const char *section, const char *key, int default_value);
    int read_profile_int(const string section, const string key, int default_value = 0);
    
    /**
    * @brief write a profile string to a ini file
    * @param section [in] name of the section,can't be NULL and empty string
    * @param key [in] name of the key pairs to value, can't be NULL and empty string
    * @param value [in] profile string value
    * @param file [in] path of ini file
    * @return 1 : success\n 0 : failure
    */
    int write_profile_string(const char *section, const char *key, const char *value);
    int write_profile_string(const string section, const string key, const string value);
   
private:
    int load_ini_file(char *buf, int *file_size);
    int newline(char c);
    int end_of_string(char c);
    int left_barce(char c);
    int isright_brace(char c );
    int parse_file(const char *section, const char *key, const char *buf,int *sec_s,int *sec_e,
					  int *key_s,int *key_e, int *value_s, int *value_e);
    
private:
    string m_path;
};

#endif //end of INI_FILE_H_

