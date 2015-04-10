/*=============================================================================
#      Filename : log.h
#   Description : 日志文件类
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-05 15:29
# Last modified : 2015-03-23 11:53
=============================================================================*/

#ifndef _LOG_H_
#define _LOG_H_

#include <string>

#define MAX_MSG_LENGTH 1024  //单个消息最大长度

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

        bool open_log(const std::string& log_path, const std::string &log_level);

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
        FILE *_file;

        LOG_LEVEL _log_level;
        std::string _base_file_path;

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
