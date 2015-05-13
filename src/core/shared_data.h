/*=============================================================================
#      Filename : shred_data.h
#   Description : 
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-24 11:30
# Last modified : 2015-03-24 11:30
=============================================================================*/
#ifndef _SHARED_DATA_H_
#define _SHARED_DATA_H_

#include <pthread.h>

class SharedData
{
    public:
        SharedData();
        ~SharedData();
    private:
        SharedData(const SharedData&);
        SharedData& operator= (const SharedData&);

    public:
        static SharedData* get_instance();

        bool create_pthread_key();
        pthread_key_t get_pthread_key();

    private:
        static SharedData _instance;

        pthread_key_t _pthread_key;
};

#endif  //_SHARED_DATA_H_
