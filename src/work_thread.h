/*=============================================================================
#      Filename : work_thread.h
#   Description : 
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-23 21:49
# Last modified : 2015-03-23 21:49
=============================================================================*/

#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include "shared_struct.h"
#include "analyse_req.h"

#include <unordered_map>

#include <event2/event.h>
#include <hiredis.h>
#include <async.h>
#include <libevent.h>

/*
struct hash64shift
{
    uint64_t operator()(uint64_t key)
    {
        key = (~key) + (key << 21); // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4); // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key; 
    }
};

struct hash_intptr
{
    uint64_t operator() (intptr_t ptr)
    {
        int64_t key  = ptr&0xFFFFFFFFFFFFFFFF;
        key = (~key) + (key << 21); // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4); // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key; 
    }
};

struct cmp_64
{
    bool operator() (const uint64_t lhs, const uint64_t rhs)
    {
        return lhs == rhs;
    }
};

struct cmp_ptr
{
    bool operator() (const intptr_t lhs, const intptr_t rhs)
    {
        return lhs == rhs;
    }
};
*/

class Subscriber;
typedef std::unordered_map<uint64_t, Subscriber *> SubMap;
typedef std::unordered_map<intptr_t, uint64_t > IntMap;

class WorkThread
{
    public:
        WorkThread();
        ~WorkThread();
    private:
        WorkThread(const WorkThread&);
        WorkThread& operator= (const WorkThread&);

    public:
        static void notify_new_conn_callback(evutil_socket_t fd, short event, void *user_data);
        static void notify_new_msg_callback(evutil_socket_t fd, short event, void *user_data);
        static void *run(void *arg);

        static void bufferevent_read_callback(struct bufferevent *bev, void *user_data);
        static void bufferevent_event_callback(struct bufferevent *bev, short events, void *user_data);

        //redis callback
        static void redis_connect_callback(const redisAsyncContext *c, int status);
        static void redis_disconnect_callback(const redisAsyncContext *c, int status);
        static void redis_login_callback(redisAsyncContext *c, void *r, void *privdata);

    public:
        bool init_work(const int work_num, PthreadInfo *work_thread, const int work_thread_num, 
                       const int max_num_per_thread, const int ping_time);
        //须在init_work之后调用
        bool asy_open_redis(const char *ip, const int port, const int db_num);

        //新连接或命令
        bool notify_new_conn(int fd){return write(_pipe_fd[1], &fd, sizeof(fd)) == sizeof(fd);}
        bool notify_new_msg(intptr_t ptr){return write(_socketpair[1], &ptr, sizeof(ptr)) == sizeof(ptr);}

        void free_data();
        int get_conn_num(){return _conn_num;}

    protected:
        void del_expire_client();

    private:
        struct event_base *_evbase;
        int    _work_num;  //工作线程编号，由主线程分配。比线程ID直观。
        int    _conn_num;  //当前连接数
        int    _ping_time;  //超时时间，下发给连接客户端

        PthreadInfo *_work_thread;  //所有线程信息
        int          _work_thread_num;  //所有线程数目

        int         _pipe_fd[2];   //新连接通知
        int         _socketpair[2];  //新消息通知

        SubMap      _sub_map;  //id to sub
        IntMap      _bev_id;  //bev to id
        AnalyseReq  _analyse_req;
        redisAsyncContext *_redis;
};

#endif  //_WORK_THREAD_H_
