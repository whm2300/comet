/*=============================================================================
#      Filename : server.h
#   Description : 管理程序，主线程。处理信号和accept新连接。
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-23 15:10
# Last modified : 2015-03-23 15:10
=============================================================================*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include "shared_struct.h"

#include <pthread.h>

#include <event2/event.h>


class Server
{
    public:
        Server();
        ~Server();
    private:
        Server(const Server&);
        Server& operator= (const Server&);

    public:
        static void log_for_libevent(int severity, const char *msg);
        static void signal_cb(evutil_socket_t sig, short events, void *user_data);
        static void del_expire_client(evutil_socket_t sig, short events, void *user_data);
        static void accept_callback(evutil_socket_t fd, short events, void *user_data);

    public:
        void analyse_config(std::string &config_path);
        bool is_daemon(){return _config_info.is_daemon;}
        bool init_server();
        bool start_work_thread();
        bool start_listen();
        int  run_server();
        void close_server();
        void check_expire_client();

    protected:
        bool init_signal_and_timer();

    private:
        Config _config_info;

        struct event_base *_evbase;
        struct event      *_timer_event;
        struct event      *_sigint_event;
        struct event      *_sigterm_event;

        PthreadInfo       *_work_thread;
        int               _work_thread_num;
        int               _main_thread_id;
};

#endif  //_SERVER_H_
