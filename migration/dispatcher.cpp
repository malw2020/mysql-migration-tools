#include "dispatcher.h"

Dispatcher* Dispatcher::instance = NULL;

Dispatcher& Dispatcher::get_instance()
{
    if (instance == NULL) {
        instance = new Dispatcher();
    } 
    
    return (*instance);
}

bool Dispatcher::replicate(MasterInfo &master, string sql_cmd)
{
    std::map<MasterInfo, MySQLAdapterSets>::iterator iterator_master_adapters;
    iterator_master_adapters = master_mysql_adapters.find(master);
    if(iterator_master_adapters == master_mysql_adapters.end())
        return false;

    MySQLAdapterSets &adapter_sets = iterator_master_adapters->second;
    unsigned int sets_total = adapter_sets.size();
    int rand_index = 0;
    switch(ReplicationPatterns::get_instance().mode)
    {
        case 1:
            rand_index = get_one_to_one_rand_seed();
            break;
        case 2:
            rand_index = get_one_to_many_rand_seed(sets_total);
            break;
        default:
            break;
    }
    
    if(false == adapter_sets[rand_index].query(sql_cmd))
    {
        return false;
    }
    
    return true;
}

int Dispatcher::get_one_to_one_rand_seed()
{
    return 0;
}

int Dispatcher::get_one_to_many_rand_seed(unsigned int max)
{
    int rand = 0;
    
    return rand;
}
