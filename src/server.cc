#include "server.h"
#include "log.h"

#include <stdio.h>

#include <string>
#include <errno.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


Server::Server():_timer_event(NULL), _sigint_event(NULL), _sigterm_event(NULL)
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

    _config_info.listen = pt.get<std::string>("server.listen");
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
    if (!SingletonLog::get_instance()->open_log(_config_info.logger_path, _config_info.logger_level)){
        printf("open log error.%s", strerror(errno));
        return false;
    }
    if (access(_config_info.pid_file_path.c_str(), F_OK) == 0){
        log_info("pidfile exist.path:%s", _config_info.pid_file_path.c_str());
        return false;
    }
    event_set_log_callback(log_for_libevent);

    _evbase = event_base_new();
    if (_evbase == NULL){
        log_fatal("%s", "create evbase error");
        return false;
    }
    log_info("use method:%s", event_base_get_method(_evbase));
    if (!init_signal_and_timer()){
        return false;
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

    int res = event_base_dispatch(_evbase);

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
    //need to fix
    //关闭子线程loop
    
    event_base_loopbreak(_evbase);
    
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
    log_debug("%s", "timeout!!");
}
