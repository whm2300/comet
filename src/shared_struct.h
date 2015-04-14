#ifndef _SHARED_STRUCT_H_
#define _SHARED_STRUCT_H_

#include <string>

typedef struct tagRedisInfo
{
    std::string ip;
    int         port;
    int         db_num;
}RedisInfo;

struct Config
{
    std::string pid_file_path;
    std::string logger_level;
    std::string logger_path;
    std::string http_url;
    std::string http_ip;
    RedisInfo   *redis_info;
    int         redis_info_count;
    int         cur_conn_index;
    int         listen;
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
}PthreadInfo;

#endif  //_SHARED_STRUCT_H_
