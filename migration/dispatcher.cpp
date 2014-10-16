#include "dispatcher.h"
#include "replication_patterns.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

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
    std::map<MasterInfo, MySQLAdapterSets*>::iterator iterator_master_adapters;
    iterator_master_adapters = master_mysql_adapters.find(master);
    if(iterator_master_adapters == master_mysql_adapters.end())
        return false;

    MySQLAdapterSets *adapter_sets = iterator_master_adapters->second;
    unsigned int sets_total = (*adapter_sets).size();
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
    
    if(false == (*adapter_sets)[rand_index].query(sql_cmd))
    {
        return false;
    }
    
    return true;
}

bool Dispatcher::load()
{
    bool result = false;
    switch(ReplicationPatterns::get_instance().mode)
    {
        case 1:
            result = load_ono_to_one();
            break;
        case 2:
            result = load_one_to_many();
            break;
        default:
            break;
    }
    
    return result;
}

bool Dispatcher::reload()
{
    unload();
    return load();
}

bool Dispatcher::unload()
{
    std::map<MasterInfo, MySQLAdapterSets*>::iterator iterator_master_adapters;
    for(iterator_master_adapters =  master_mysql_adapters.begin(); iterator_master_adapters != master_mysql_adapters.end(); ++iterator_master_adapters)
    {
        MySQLAdapterSets *adapter_sets = iterator_master_adapters->second;
        for(unsigned int index=0; index<adapter_sets->size(); ++index)
        {
            (*adapter_sets)[index].disconnect();
        }
    }
    return true;
}

int Dispatcher::get_one_to_one_rand_seed()
{
    // default index 0
    return 0;
}

int Dispatcher::get_one_to_many_rand_seed(unsigned int max)
{
    int rand_value = 0;
    
    srand((int) time(0));
    rand_value = rand()%max;
    
    return rand_value;
}

bool Dispatcher::load_ono_to_one()
{
    cout<<"enter load_ono_to_one"<<endl;
    std::vector<SourceNode>& source_nodes = ReplicationPatterns::get_instance().source_nodes;
    for(unsigned int index=0; index<source_nodes.size(); ++index)
    {
        MasterInfo master;
        master.ip   = source_nodes[index].ip;
        master.port = source_nodes[index].port;
        
        MySQLAdapterSets *adapter_sets = new MySQLAdapterSets();
        std::vector<DestinationNode>& destination_nodes = ReplicationPatterns::get_instance().destination_nodes;
        if(0 == destination_nodes.size())
            return false;
        
        cout<<"init adapter "<<destination_nodes[0].ip<<"  "<<destination_nodes[0].port<<endl;
        MySQLAdapter adapter(destination_nodes[0].ip, destination_nodes[0].port, 
                             destination_nodes[0].user, destination_nodes[0].password, 
                             destination_nodes[0].database);  
        if(false == adapter.connect())
            return false;
        
        adapter_sets->push_back(adapter);
        cout<<"exit load_ono_to_one .."<<endl;
        
        master_mysql_adapters.insert(std::pair<MasterInfo, MySQLAdapterSets*>(master, adapter_sets));
        cout<<"exit load_ono_to_one ..."<<endl;
        
    }
    cout<<"exit load_ono_to_one"<<endl;
     
    return true;
}

bool Dispatcher::load_one_to_many()
{
    std::vector<SourceNode>& source_nodes = ReplicationPatterns::get_instance().source_nodes;
    for(unsigned int index=0; index<source_nodes.size(); ++index)
    {
        MasterInfo master;
        master.ip   = source_nodes[index].ip;
        master.port = source_nodes[index].port;
        
        MySQLAdapterSets *adapter_sets = new MySQLAdapterSets();
        std::vector<DestinationNode>& destination_nodes = ReplicationPatterns::get_instance().destination_nodes;
        if(0 == destination_nodes.size())
            return false;
        
        for (unsigned int dest_index=0; dest_index<destination_nodes.size(); ++dest_index)
        {
             MySQLAdapter adapter(destination_nodes[dest_index].ip, destination_nodes[dest_index].port, 
                             destination_nodes[dest_index].user, destination_nodes[dest_index].password, 
                             destination_nodes[dest_index].database);  
             if (false == adapter.connect())
                 return false;
        
             adapter_sets->push_back(adapter);
        }
        
        master_mysql_adapters.insert(std::pair<MasterInfo, MySQLAdapterSets*>(master, adapter_sets));
    }
    
    return true;
}
