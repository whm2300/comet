#include "subscriber.h"

#include <sys/time.h>

#include <event2/bufferevent.h>

Subscriber::Subscriber():_last_heartbeat_time(0), _id(0), bev(NULL)
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
