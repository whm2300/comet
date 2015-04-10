#ifndef _SHARED_STRUCT_H_
#define _SHARED_STRUCT_H_

#include <string>

struct Config
{
    std::string pid_file_path;
    std::string logger_level;
    std::string logger_path;
    std::string redis_ip;
    std::string http_url;
    std::string http_ip;
    int         listen;
    int         redis_port;
    int         redis_db;
    int         work_thread_num;  //工作线程数目
    int         max_num_per_thread;  //每个工作线程最大连接数
    int         heartbeat_time;  //心跳包发送间隔时间，服务器按2*heartbeat_time做超时检测。
    bool        is_daemon;
};

class WorkThread;
typedef struct tag_PthreadInfo
{
    WorkThread   *work_thread;
    pthread_t    thread_id;
    //int          notify_new_conn;
    //int          notify_new_msg;
}PthreadInfo;

#endif  //_SHARED_STRUCT_H_
