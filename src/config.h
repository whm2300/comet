#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

struct Config
{
    std::string pid_file_path;
    std::string logger_level;
    std::string logger_path;
    std::string listen;
    std::string redis_ip;
    int         redis_port;
    int         redis_db;
    int         work_thread_num;  //工作线程数目
    int         max_num_per_thread;  //每个工作线程最大连接数
    int         heartbeat_time;  //心跳包发送间隔时间，服务器按2*heartbeat_time做超时检测。
    bool        is_daemon;
};

#endif  //_CONFIG_H_
