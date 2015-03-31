#include "subscriber.h"

#include <sys/time.h>

#include <event2/bufferevent.h>

Subscriber::Subscriber():_last_heartbeat_time(0), _id(0), _bev(NULL)
{}

Subscriber::~Subscriber()
{}

void Subscriber::update_heartbeat()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != -1){
       _last_heartbeat_time = tv.tv_sec; 
    }
}

void Subscriber::set_id_bev(uint64_t id, struct bufferevent *bev)
{
    _id = id;
    _bev = bev;
    update_heartbeat();
}
