#include "shared_data.h"
#include "log.h"

#include <string.h>

SharedData SharedData::_instance;

SharedData::SharedData()
{}

SharedData::~SharedData()
{
    pthread_key_delete(_pthread_key);
}

SharedData* SharedData::get_instance()
{
    return &_instance;
}
bool SharedData::create_pthread_key()
{
    int res = pthread_key_create(&_pthread_key, NULL);
    if (res != 0){
        return false;
    }
    return true;
}

pthread_key_t SharedData::get_pthread_key()
{
    return _pthread_key;
}
