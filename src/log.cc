#include "log.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
//#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>
#include <cstdarg>

SingletonLog *SingletonLog::_instance = new SingletonLog;
SingletonLog::garbo SingletonLog::_garbo;

SingletonLog *SingletonLog::get_instance()
{
    return _instance;
}
SingletonLog::SingletonLog()
{}

SingletonLog::~SingletonLog()
{
}

bool SingletonLog::open_log(const std::string& log_path, const std::string &log_level)
{
    _base_file_path = log_path;
    _log_level = get_log_level(log_level);

    _log_fd = open(_base_file_path.c_str(), O_WRONLY | O_APPEND | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (_log_fd == -1){
        return false;
    }

    return true;
}

enum SingletonLog::LOG_LEVEL SingletonLog::get_log_level(const std::string &log_level)
{
    if (log_level == std::string("fatal")){
        return FATAL;
    }
    else if (log_level == std::string("error")){
        return ERROR;
    }
    else if (log_level == std::string("warn")){
        return WARN;
    }
    else if (log_level == std::string("info")){
        return INFO;
    }
    else if (log_level == std::string("debug")){
        return DEBUG;
    }
    else if (log_level == std::string("trace")){
        return TRACE;
    }
    else{
        return TRACE;
    }
}

bool SingletonLog::close_log()
{
    if (close(_log_fd) == -1)
      return false;
    return true;
}

void SingletonLog::write_log(LOG_LEVEL log_level, const char *fmt, ...)
{
    if (log_level > _log_level)
      return ;

    time_t time;
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    tm = localtime(&time);

    char buffer[MAX_MSG_LENGTH];
    int len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%06d] [%lx] %s:", tm->tm_year + 1900, tm->tm_mon + 1, 
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec), 
                pthread_self(), SingletonLog::get_level_name(log_level));
    if (len < 0)
      return ;

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer+len, MAX_MSG_LENGTH-len, fmt, ap);
    va_end(ap);

    len = strlen(buffer);
    char *end = buffer + len;
    *end ++ = '\n';
    *end = '\0';
    write(_log_fd, buffer, len + 1);
}

inline const char *SingletonLog::get_level_name(LOG_LEVEL level)
{
    switch (level){
        case SingletonLog::FATAL:
            return "[FATAL]";
        case SingletonLog::ERROR:
            return "[ERROR]";
        case SingletonLog::WARN:
            return "[WARN ]";
        case SingletonLog::INFO:
            return "[INFO ]";
        case SingletonLog::DEBUG:
            return "[DEBUG]";
        case SingletonLog::TRACE:
            return "[TRACE]";
        default:
                return "";
    }
}
