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
#include "comet.pb.h"

#include <unordered_map>

#include <event2/event.h>
#include <hiredis.h>
#include <async.h>
#include <libevent.h>


class Subscriber;
typedef std::unordered_map<uint64_t, Subscriber *> SubMap;
typedef std::unordered_map<intptr_t, uint64_t > IntMap;

class WorkThread;
typedef struct tagOnlineInfo
{
    uint64_t           id;  //上线id
    struct bufferevent *bev;
    WorkThread         *work_thread;
}OnlineInfo;

typedef struct tagOfflineInfo
{
    uint64_t           id;  //下线id
    WorkThread         *work_thread;
}OfflineInfo;

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
        static void redis_trans_callback(redisAsyncContext *c, void *r, void *privdata);
        //上线通知，从redis中检测是否存在好友音箱，下发好友音箱在线/离线状态
        static void redis_online_notify(redisAsyncContext *c, void *r, void *privdata);
        //下线通知，检测redis好友列表，向在线好友下发下线通知
        static void redis_offline_notify(redisAsyncContext *c, void *r, void *privdata);

        //http callback
        static void http_login_read(struct bufferevent *bev, void *ctx);
        static void http_login_write(struct bufferevent *bev, void *ctx);
        static void http_login_event(struct bufferevent *bev, short event, void *ctx);

        static void http_trans_read(struct bufferevent *bev, void *ctx);
        static void http_trans_write(struct bufferevent *bev, void *ctx);
        static void http_trans_event(struct bufferevent *bev, short event, void *ctx);

    public:
        bool init_work(const int work_num, PthreadInfo *work_thread, const int work_thread_num, 
                       const int max_num_per_thread, const int ping_time, const std::string &http_url, const std::string &http_ip);
        //须在init_work之后调用
        bool asy_open_redis(const char *ip, const int port, const int db_num);

        //新连接或命令
        bool notify_new_conn(int fd);
        bool notify_new_msg(intptr_t ptr);
        void notify_del_id(uint64_t id);

        bool check_id_is_online(uint64_t id);

        void free_data();
        int get_conn_num(){return _conn_num;}

    protected:
        void del_expire_client();
        bool setnonblock(int fd);
        bool http_login_check(LoginData *login_data);
        bool http_trans_check(TransData *trans_data);
        bool send_message(TransData *trans_data);

    private:
        struct event_base *_evbase;
        int    _work_num;  //工作线程编号，由主线程分配。比线程ID直观。
        int    _conn_num;  //当前连接数
        int    _ping_time;  //超时时间，下发给连接客户端

        PthreadInfo *_work_thread;  //所有线程信息
        int          _work_thread_num;  //所有线程数目

        int         _new_conn_fd[2];   //新连接通知
        int         _new_msg_fd[2];  //新消息通知

        std::string _http_url;
        std::string _http_ip;

        SubMap      _sub_map;  //id to sub
        IntMap      _bev_id;  //bev to id
        AnalyseReq  _analyse_req;
        redisAsyncContext *_redis;

        eglcomet::OnlineStatus _online_status;  //好友音箱的在线状态
        eglcomet::OnlineStatus _me_status;  //自己的在线状态
};

#endif  //_WORK_THREAD_H_
