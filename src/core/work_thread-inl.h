#ifndef _WORK_THREAD_INL_H_
#define _WORK_THREAD_INL_H_

inline bool WorkThread::notify_new_conn(int fd)
{
        return write(_new_conn_fd[1], &fd, sizeof(fd)) == sizeof(fd);
}

inline bool WorkThread::check_id_is_online(uint64_t id)
{
        return _sub_map.find(id) != _sub_map.end();
}

#endif  //_WORK_THREAD_INL_H_
