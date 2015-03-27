/*=============================================================================
#      Filename : subscriber.h
#   Description : 
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-25 10:40
# Last modified : 2015-03-25 10:40
=============================================================================*/
#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include <string>

class Subscriber
{
    public:
        Subscriber();
        ~Subscriber();

        void set_id(uint64_t id){_id = id;}
        bool is_valid(){return _id != 0;}

        void update_heartbeat();
        int64_t get_heartbeat_time(){return _last_heartbeat_time;}
    private:
        Subscriber(const Subscriber&);
        Subscriber& operator= (const Subscriber&);

    private:
        int64_t             _last_heartbeat_time;
        int64_t             _id;  //0 未通过验证。
        struct bufferevent  *bev;
        //std::string         _ip;
};

#endif  //_SUBSCRIBER_H_
