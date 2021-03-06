#include "server.h"

#include <stdio.h>
#include <string>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "log.h"
#include "shared_data.h"
#include "work_thread.h"
#include "work_thread-inl.h"


Server::Server():_timer_event(NULL), _sigint_event(NULL), 
    _sigterm_event(NULL), _usr1_event(NULL), _work_thread(NULL), _work_thread_num(0), _main_thread_id(0)
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
    _config_info.logger_folder = pt.get<std::string>("logger.folder");

    _config_info.listen = pt.get<int>("server.listen");
    _config_info.work_thread_num = pt.get<int>("server.work_thread");
    _config_info.max_num_per_thread = pt.get<int>("server.max_num_per_thread");
    _config_info.heartbeat_time = pt.get<int>("server.heartbeat_time");

    std::string ip("redis.ip%d");
    std::string port("redis.port%d");
    std::string db_num("redis.db_num%d");
    char buffer[64];
    _config_info.redis_info_count = pt.get<int>("redis.redis_info_count");
    _config_info.cur_conn_index = 0;
    _config_info.redis_info = new RedisInfo[_config_info.redis_info_count];
    for (int i = 0; i < _config_info.redis_info_count; i++){
        sprintf(buffer, ip.c_str(), i);
        _config_info.redis_info[i].ip = pt.get<std::string>(buffer);
        sprintf(buffer, port.c_str(), i);
        _config_info.redis_info[i].port = pt.get<int>(buffer);
        sprintf(buffer, db_num.c_str(), i);
        _config_info.redis_info[i].db_num = pt.get<int>(buffer);
    }

    _config_info.http_url = pt.get<std::string>("http.url");

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
    //get http ip
    struct hostent *h;
    if ((h = gethostbyname(_config_info.http_url.c_str())) == NULL){
        return false;
    }
    char ip[32];
    memset(ip, 0, 32);
    inet_ntop(h->h_addrtype, h->h_addr, ip, sizeof(ip));
    _config_info.http_ip = ip;

    //shared data. 
    if (!SharedData::get_instance()->create_pthread_key()){
        return false;
    }

    //log
    if (!SingletonLog::get_instance()->open_log(_config_info.logger_folder, _config_info.logger_level)){
        printf("open log error.%s", strerror(errno));
        return false;
    }

    //libevent
    event_set_log_callback(log_for_libevent);
    event_enable_debug_mode();
    _evbase = event_base_new();
    if (_evbase == NULL){
        return false;
    }
    ThreadInfo *thread_info = new ThreadInfo;
    thread_info->thread_id = _main_thread_id;
    thread_info->evbase = _evbase;
    if (pthread_setspecific(SharedData::get_instance()->get_pthread_key(), thread_info) != 0){
        return false;
    }
    log_info("use method:%s, %ld", event_base_get_method(_evbase), _evbase);
    if (!init_signal_and_timer()){
        return false;
    }

    //pid file
    if (access(_config_info.pid_file_path.c_str(), F_OK) == 0){
        log_info("pidfile exist. path:%s", _config_info.pid_file_path.c_str());
        return false;
    }

    log_info("pidfile:%s", _config_info.pid_file_path.c_str());
    log_info("logger.level:%s", _config_info.logger_level.c_str());
    log_info("logger.folder:%s", _config_info.logger_folder.c_str());
    log_info("server.listen:%d", _config_info.listen);
    log_info("server.work_thread:%d", _config_info.work_thread_num);
    log_info("server.max_num_per_thread:%d", _config_info.max_num_per_thread);
    log_info("server.heartbeat_time:%d", _config_info.heartbeat_time);
    log_info("http.url:%s", _config_info.http_url.c_str());
    for (int i = 0; i < _config_info.redis_info_count; i++){
        log_info("ip%d:%s", i, _config_info.redis_info[i].ip.c_str());
        log_info("port%d:%d", i, _config_info.redis_info[i].port);
        log_info("db_num%d:%d", i, _config_info.redis_info[i].db_num);
    }


    return true;
}

bool Server::start_work_thread()
{
    _work_thread_num = _config_info.work_thread_num;
    _work_thread = new PthreadInfo[_work_thread_num];
    gettimeofday(&_last_connect_time, NULL);
    for (int i = 0; i < _work_thread_num; ++i){
        _work_thread[i].work_thread = new WorkThread();
        if (!_work_thread[i].work_thread->init_work(i+1, _work_thread, _work_thread_num, 
                        _config_info.max_num_per_thread, _config_info.heartbeat_time, _config_info.http_url, _config_info.http_ip)
                    || !_work_thread[i].work_thread->asy_open_redis(_config_info.redis_info[_config_info.cur_conn_index].ip.c_str(), 
                _config_info.redis_info[_config_info.cur_conn_index].port, _config_info.redis_info[_config_info.cur_conn_index].db_num)){
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

    if (!start_listen()){
        return -1;
    }

    sigset_t bset;
    sigemptyset(&bset);
    sigaddset(&bset, SIGINT);
    sigaddset(&bset, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &bset, NULL) != 0){
        log_error("%s", "block SIG error");
        return -1;
    }
    //start work thread
    if (!start_work_thread()){
        return -1;
    }
    if (sigprocmask(SIG_UNBLOCK, &bset, NULL) != 0){
        log_error("%s", "unblock SIG error");
        return -1;
    }

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
    _usr1_event = evsignal_new(_evbase, SIGUSR1, reconnect_redis, this);
    if (_usr1_event == NULL || event_add(_usr1_event, NULL) < 0){
        log_error("init SIGUSR1 error");
        return false;
    }
    _usr2_event = evsignal_new(_evbase, SIGUSR2, rotate_log, this);
    if (_usr2_event == NULL || event_add(_usr2_event, NULL) < 0) {
        log_error("init SIGUSR2 error");
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

void Server::rotate_log(evutil_socket_t sig, short events, void *user_data)
{
    Server *server = (Server *)user_data;
    if (!SingletonLog::get_instance()->open_log(server->_config_info.logger_folder, server->_config_info.logger_level)){
        log_error("open log error.%s", strerror(errno));
    }
}

void Server::reconnect_redis(evutil_socket_t sig, short events, void *user_data)
{
    Server *server = (Server *)user_data;
    timeval cur_time;
    gettimeofday(&cur_time, NULL);
    if (cur_time.tv_sec - server->_last_connect_time.tv_sec > 10){  //重连时间间隔为10s
        server->_last_connect_time = cur_time;
        log_info("redis %d disconnect, start connect redis %d", server->_config_info.cur_conn_index, 
                    server->_config_info.cur_conn_index = (server->_config_info.cur_conn_index + 1) % server->_config_info.redis_info_count);

        for (int i = 0; i < server->_work_thread_num; i++){
            if (!server->_work_thread[i].work_thread->asy_open_redis(
                            server->_config_info.redis_info[server->_config_info.cur_conn_index].ip.c_str(), 
                            server->_config_info.redis_info[server->_config_info.cur_conn_index].port, 
                            server->_config_info.redis_info[server->_config_info.cur_conn_index].db_num)){
                log_error("reconnect error");
                kill(getpid(), SIGTERM);
            }
        }
    }
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
    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
        log_error("set socket opt fail. %s", strerror(errno));
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

    if (min_count < server->_config_info.max_num_per_thread){
        int conn_fd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (conn_fd != -1){
            server->_work_thread[min_index].work_thread->notify_new_conn(conn_fd);
        }
    }

    /*
       int conn_fd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (conn_fd != -1){
        int thread_index = conn_fd % server->_work_thread_num;
        if (server->_work_thread[thread_index].work_thread->get_conn_num() < server->_config_info.max_num_per_thread){
            server->_work_thread[thread_index].work_thread->notify_new_conn(conn_fd);
        }
        else{
            close(conn_fd);
        }
    }
    */
}
