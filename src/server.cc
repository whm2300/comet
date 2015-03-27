#include "server.h"
#include "log.h"
#include "shared_data.h"
#include "work_thread.h"

#include <stdio.h>

#include <string>
#include <errno.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


Server::Server():_timer_event(NULL), _sigint_event(NULL), 
    _sigterm_event(NULL), _work_thread(NULL), _work_thread_num(0), _main_thread_id(0)
{}

Server::~Server()
{}

void Server::analyse_config(std::string &config_path)
{
    using namespace boost::property_tree;
    ptree pt;
    read_info(config_path.c_str(), pt);

    _config_info.pid_file_path = pt.get<std::string>("pidfile");

    _config_info.logger_level = pt.get<std::string>("logger.level");
    _config_info.logger_path = pt.get<std::string>("logger.path");

    _config_info.listen = pt.get<int>("server.listen");
    _config_info.work_thread_num = pt.get<int>("server.work_thread");
    _config_info.max_num_per_thread = pt.get<int>("server.max_num_per_thread");
    _config_info.heartbeat_time = pt.get<int>("server.heartbeat_time");

    _config_info.redis_ip = pt.get<std::string>("redis.ip");
    _config_info.redis_port = pt.get<int>("redis.port");
    _config_info.redis_db = pt.get<int>("redis.db_num");

    std::string is_daemon = pt.get<std::string>("daemon");
    if (is_daemon == std::string("on")){
        _config_info.is_daemon = true;
    }
    else{
        _config_info.is_daemon = false;
    }
}

bool Server::init_server()
{
    //shared data. 
    if (!SharedData::get_instance()->create_pthread_key()){
        return false;
    }
    if (pthread_setspecific(SharedData::get_instance()->get_pthread_key(), &_main_thread_id) != 0){
        return false;
    }

    //log
    if (!SingletonLog::get_instance()->open_log(_config_info.logger_path, _config_info.logger_level)){
        printf("open log error.%s", strerror(errno));
        return false;
    }

    //pid file
    if (access(_config_info.pid_file_path.c_str(), F_OK) == 0){
        log_info("pidfile exist. path:%s", _config_info.pid_file_path.c_str());
        return false;
    }

    //libevent
    event_set_log_callback(log_for_libevent);
    event_enable_debug_mode();
    _evbase = event_base_new();
    if (_evbase == NULL){
        log_fatal("%s", "create evbase error");
        return false;
    }
    log_info("use method:%s, %ld", event_base_get_method(_evbase), _evbase);
    if (!init_signal_and_timer()){
        return false;
    }

    sigset_t bset;
    sigemptyset(&bset);
    sigaddset(&bset, SIGINT);
    sigaddset(&bset, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &bset, NULL) != 0){
        log_error("%s", "block SIG error");
        return false;
    }
    //init work thread
    if (!init_work_thread()){
        return false;
    }
    if (sigprocmask(SIG_UNBLOCK, &bset, NULL) != 0){
        log_error("%s", "unblock SIG error");
        return false;
    }

    return true;
}

bool Server::init_work_thread()
{
    _work_thread_num = _config_info.work_thread_num;
    _work_thread = new PthreadInfo[_work_thread_num];
    for (int i = 0; i < _work_thread_num; ++i){
        _work_thread[i].work_thread = new WorkThread();
        if (!_work_thread[i].work_thread->init_work(i+1, _work_thread, _work_thread_num, 
                        _config_info.max_num_per_thread, _config_info.heartbeat_time)
                    || !_work_thread[i].work_thread->asy_open_redis(_config_info.redis_ip.c_str(), 
                _config_info.redis_port, _config_info.redis_db)){
            log_error("init work thread error. %d", i);
            return false;
        }
        int res = pthread_create(&_work_thread[i].thread_id, 
                    NULL, WorkThread::run, _work_thread[i].work_thread);
        if (res != 0){
            log_error("Could not craete thread. %s", strerror(errno));
            return false;
        }
    }
    return true;
}

int Server::run_server()
{
    {
        int pidfile = open(_config_info.pid_file_path.c_str(), O_WRONLY | O_CREAT, 
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (pidfile == -1){
            log_error("Could not open pidfile. %s", strerror(errno));
            return -1;
        }
        char buf[21] = {0};
        sprintf(buf, "%d", getpid());
        write(pidfile, buf, strlen(buf));
        ::close(pidfile);
    }

    start_listen();
    int res = event_base_dispatch(_evbase);

    log_debug("%s", "main loop exit");
    remove(_config_info.pid_file_path.c_str());

    return res;
}

bool Server::init_signal_and_timer()
{
    signal(SIGPIPE, SIG_IGN);

    _sigint_event = evsignal_new(_evbase, SIGINT, signal_cb, this);
    if (_sigint_event == NULL || event_add(_sigint_event, NULL) < 0){
        log_error("%s", "Could not create/add SIGINT event");
        return false;
    }
    _sigterm_event = evsignal_new(_evbase, SIGTERM, signal_cb, this);
    if (_sigterm_event == NULL || event_add(_sigterm_event, NULL) < 0 ){
        log_error("%s", "Could not create/add SIGTERM event");
        return false;
    }
    _timer_event = event_new(_evbase, -1, EV_PERSIST, del_expire_client, this);
    {
        struct timeval tv;
        tv.tv_sec = _config_info.heartbeat_time*2;
        tv.tv_usec = 0;
        if (_timer_event == NULL || evtimer_add(_timer_event, &tv) < 0){
            log_error("%s", "could not create/add a timer event");
            return false;
        }
    }
    return true;
}

void Server::log_for_libevent(int severity, const char *msg)
{
    switch (severity)
    {
        case EVENT_LOG_DEBUG:
            log_debug("%s", msg);
            break;

        case EVENT_LOG_MSG:
            log_info("%s", msg);
            break;

        case EVENT_LOG_WARN:
            log_warn("%s", msg);

        case EVENT_LOG_ERR:
            log_error("%s", msg);
            break;

        default:
            log_error("%s, %d", "libevent log level error", severity);
            break;
    }
}

void Server::close_server()
{
    event_base_loopbreak(_evbase);

    for (int i = 0; i < _work_thread_num; ++i){
        int stop = -1;
        _work_thread[i].work_thread->notify_new_conn(stop);  //send stop signal
        pthread_join(_work_thread[i].thread_id, NULL);
        _work_thread[i].work_thread->free_data();
        delete _work_thread[i].work_thread;
    }
    delete []_work_thread;
    
    event_free(_sigint_event);
    event_free(_sigterm_event);
    event_free(_timer_event);
    event_base_free(_evbase);

    log_info("%s", "eglcomet exit!!");
}

void Server::signal_cb(evutil_socket_t sig, short events, void *user_data)
{
    Server *server = (Server *)user_data;
    server->close_server();
}

void Server::del_expire_client(evutil_socket_t sig, short events, void *user_data)
{
    Server *server = (Server *)user_data;
    server->check_expire_client();
}

void Server::check_expire_client()
{
    for (int i = 0; i < _work_thread_num; ++i){
        int cmd = -2;
        _work_thread[i].work_thread->notify_new_conn(cmd);
    }
}

bool Server::start_listen()
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)_config_info.listen);
    addr.sin_addr.s_addr = INADDR_ANY;

    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd == -1){
        log_error("get listen socket error. %s", strerror(errno));
        return false;
    }
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        log_error("bind addr error. %s", strerror(errno));
        return false;
    }
    if (listen(listen_fd, 5) != 0){
        log_error("listen error. %s", strerror(errno));
        return false;
    }

    struct event *listen_event = event_new(_evbase, listen_fd, EV_READ | EV_PERSIST, accept_callback, this);
    event_add(listen_event, NULL);
    log_info("start listen, listen at:%d", _config_info.listen);
    return true;
}

void Server::accept_callback(evutil_socket_t fd, short events, void *user_data)
{
    Server *server = (Server *)user_data;
    int min_count = server->_work_thread[0].work_thread->get_conn_num();
    int min_index = 0;
    for (int i = 1; i < server->_work_thread_num; ++i){
        if (server->_work_thread[i].work_thread->get_conn_num() < min_count){
            min_index = i;
            min_count = server->_work_thread[i].work_thread->get_conn_num();
        }
    }

    if (min_count > server->_config_info.max_num_per_thread){
        return ;
    }

    int conn_fd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
    server->_work_thread[min_index].work_thread->notify_new_conn(conn_fd);
}
