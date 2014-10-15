#include "dispatcher.h"

Dispatcher* Dispatcher::instance = NULL;

Dispatcher& Dispatcher::get_instance()
{
    if (instance == NULL) {
        instance = new Dispatcher();
    } 
    
    return (*instance);
}

bool Dispatcher::replicate(string sqlCmd) {
                 
    return false;
}