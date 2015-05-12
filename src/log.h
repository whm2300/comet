/*=============================================================================
#      Filename : log.h
#   Description : 日志文件类
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-05 15:29
# Last modified : 2015-04-08 21:26
=============================================================================*/

#ifndef _LOG_H_
#define _LOG_H_

#include <event2/event.h>

#include <string>

#define MAX_MSG_LENGTH 1024  //单个消息最大长度

typedef struct tagThreadInfo
{
    int32_t thread_id;
    struct event_base *evbase;
}ThreadInfo;

class SingletonLog
{
    public:
        enum LOG_LEVEL 
        {
            FATAL = 0,
            ERROR = 1,
            WARN = 2,
            INFO = 3,
            DEBUG = 4,
            TRACE = 5,
        };

    public:
        static SingletonLog *get_instance();

        bool open_log(const std::string& log_folder, const std::string &log_level);

        bool close_log();

        void write_log(LOG_LEVEL log_level, const char *fmt, ...);

    private:
        SingletonLog();
        ~SingletonLog();
        SingletonLog(const SingletonLog&);
        SingletonLog& operator= (const SingletonLog&);

        enum LOG_LEVEL get_log_level(const std::string &log_level);
        inline static const char *get_level_name(LOG_LEVEL level);

    private:
        static SingletonLog *_instance;

        int _log_fd;
        int _old_fd;
        FILE *_file;

        LOG_LEVEL _log_level;

        class garbo
        {
            public:
                ~garbo()
                {
                    if (SingletonLog::_instance != NULL){
                        delete SingletonLog::_instance;
                        SingletonLog::_instance = NULL;
                    }
                }
        };
        static garbo _garbo;
};

#define log_fatal(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::FATAL, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_error(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::ERROR, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_warn(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::WARN, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_info(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::INFO, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_debug(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::DEBUG, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_trace(fmt, args...) \
    SingletonLog::get_instance()->write_log(SingletonLog::TRACE, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)


#endif //_LOG_H_
