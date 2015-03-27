#include "work_thread.h"
#include "log.h"
#include "shared_data.h"
#include "subscriber.h"

#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>

WorkThread::WorkThread():_evbase(NULL), _work_num(0), _conn_num(0),_redis(NULL)
{}

WorkThread::~WorkThread()
{}

bool WorkThread::init_work(const int work_num, PthreadInfo *work_thread, const int work_thread_num, 
                           const int max_num_per_thread, const int ping_time)
{
    if (pipe(_pipe_fd) != 0){
        log_error("%s", "create pipe file description error");
        return false;
    }
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, _socketpair) != 0){
        log_error("%s", "craete socket pair error");
        return false;
    }
    _work_num = work_num;
    _work_thread = work_thread;
    _work_thread_num = work_thread_num;
    _ping_time = ping_time;
    _sub_map.rehash(max_num_per_thread * 1.2);
    _bev_id.rehash(max_num_per_thread * 1.2);

    _evbase = event_base_new();
    if (_evbase == NULL){
        log_error("%s", "Could not create evbase");
        return false;
    }

    struct event *notify = event_new(_evbase, _pipe_fd[0], EV_READ | EV_PERSIST, notify_new_conn_callback, this);
    if (notify == NULL){
        log_error("%s", "Could not create notify new connection event");
        return false;
    }
    if (event_add(notify, NULL) != 0){
        log_error("%s, %d", "add notify new connection event error", work_num);
        return false;
    }

    notify = event_new(_evbase, _socketpair[0], EV_READ | EV_PERSIST, notify_new_msg_callback, this);
    if (notify == NULL){
        log_error("%s", "Could not create notify new msg event");
        return false;
    }
    if (event_add(notify, NULL) != 0){
        log_error("%s, %d", "add notify new msg event error", work_num);
    }

    return true;
}

bool WorkThread::asy_open_redis(const char *ip, const int port, const int db_num)
{
    _redis = redisAsyncConnect(ip, port);
    if (_redis->err){
        log_info("redis connect error:%s", _redis->errstr);
        return false;
    }
    redisLibeventAttach(_redis, _evbase);
    redisAsyncSetConnectCallback(_redis, redis_connect_callback);
    redisAsyncSetDisconnectCallback(_redis, redis_disconnect_callback);
    redisAsyncCommand(_redis, NULL, NULL, "SELECT %d", db_num);
    return true;
}

void WorkThread::notify_new_conn_callback(evutil_socket_t fd, short event, void *user_data)
{
    WorkThread *work_thread = (WorkThread *)user_data;
    int data;
    if (read(fd, &data, sizeof(data)) != sizeof(data)){
        return ;
    }

    if (data > 0){  // new connection
        struct bufferevent *bev = bufferevent_socket_new(work_thread->_evbase, data, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, bufferevent_read_callback, NULL, bufferevent_event_callback, user_data);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
        __sync_fetch_and_add(&work_thread->_conn_num, 1);
    }
    else{  //cmd
        if (data == -1){  //stop cmd
            event_base_loopbreak(work_thread->_evbase);
        }
        else if (data == -2){
            work_thread->del_expire_client();
        }
    }
}

void WorkThread::del_expire_client()
{
    IntMap::iterator bev_pos = _bev_id.begin();
    SubMap::iterator id_pos;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1){
        return ;
    }

    while (bev_pos != _bev_id.end()){
        id_pos = _sub_map.find(bev_pos->second);
        if (id_pos != _sub_map.end()){
            // timeout
            if ((tv.tv_sec - id_pos->second->get_heartbeat_time()) > 2*_ping_time){
                log_info("id:%ld, timeout", id_pos->first);
                delete id_pos->second;
                bufferevent_free((struct bufferevent *)bev_pos->first);
                bev_pos = _bev_id.erase(bev_pos);
                id_pos = _sub_map.erase(id_pos);
            }
            else{
                ++bev_pos;
            }
        }
        else{
            ++bev_pos;
            log_error("%s", "bev and id find, not find sub. logic error");
        }
    }

    //statistical data
    log_info("current bev client size:%d, current id client size:%d", _bev_id.size(), _sub_map.size());
}

void WorkThread::notify_new_msg_callback(evutil_socket_t fd, short event, void *user_data)
{
    log_debug("%s", "new msg");
}

void WorkThread::bufferevent_read_callback(struct bufferevent *bev, void *user_data)
{
    WorkThread *work_thread = (WorkThread *)user_data;
    uint64_t id = 0;
    IntMap::iterator it = work_thread->_bev_id.find((intptr_t)bev);
    if (it != work_thread->_bev_id.end()){
        id = it->second;
    }
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    size_t len = evbuffer_get_length(input);

    while (len > 3)  //min data bag length is 4
    {
        log_info("get req data from %ld", id);
        if (!work_thread->_analyse_req.analyse_data(input, output)){
            break;
        }
        len = evbuffer_get_length(input);
        log_debug("after handle data size:%d", len);

        LoginData *login_data = work_thread->_analyse_req.get_login_data();
        if (login_data != NULL){
            login_data->work_thread = (intptr_t)work_thread;
            login_data->bev = (intptr_t)bev;
            redisAsyncCommand(work_thread->_redis, redis_login_callback,
                        (char*)login_data, "GET sToken:%ld", login_data->id);
            log_debug("%s, bev:%lx", "get login data, start check.", bev);

        }

        if (work_thread->_analyse_req.is_heart() && id != 0){
            SubMap::iterator pos = work_thread->_sub_map.find(id);
            if (pos != work_thread->_sub_map.end()){
                pos->second->update_heartbeat();
                log_debug("%s", "update hearbeat time");
            }
        }

        TransData *trans_data = work_thread->_analyse_req.get_trans_data();
        if (trans_data != NULL){
            log_error("%s", "asdfasdfasdfasfdsfsadf");
        }
    }
}

void WorkThread::bufferevent_event_callback(struct bufferevent *bev, short events, void *user_data)
{
    WorkThread *work_thread = (WorkThread *)user_data;
    struct evbuffer *input = bufferevent_get_input(bev);
    
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)){
        uint64_t id = 0;
        IntMap::iterator it = work_thread->_bev_id.find((intptr_t)bev);
        if (it != work_thread->_bev_id.end()){
            id = it->second;
            SubMap::iterator sub_pos = work_thread->_sub_map.find(id);
            if (sub_pos != work_thread->_sub_map.end()){
                delete sub_pos->second;
                work_thread->_sub_map.erase(sub_pos);
            }
            work_thread->_bev_id.erase(it);
        }

        size_t len = evbuffer_get_length(input);
        if (events & BEV_EVENT_EOF){
            log_info("get a close from %d, we drained %d bytes from it.", id, len);
        }
        else{
            log_info("get a error from %d, we drained %d bytes from it.", id, len);
        }

        __sync_fetch_and_sub(&work_thread->_conn_num, 1);
        bufferevent_free(bev);
    }
}

void *WorkThread::run(void *arg)
{
    WorkThread *work = (WorkThread *)arg;

    if (pthread_setspecific(SharedData::get_instance()->get_pthread_key(), &work->_work_num) != 0){
        log_error("id:%d, set thread private data error. %s, thread exit!", work->_work_num, strerror(errno));
        return NULL;
    }

    log_info("%s", "work thread start loop");
    int res = event_base_dispatch(work->_evbase);
    log_info("%s. exit code:%d", "work thread exit loop", res);

    return NULL;
}

void WorkThread::free_data()
{
    event_base_free(_evbase);
    SubMap::iterator it = _sub_map.begin();
    while (it != _sub_map.end()){
        delete it->second;
    }
}

void WorkThread::redis_connect_callback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK){
        log_info("redis connect error:%s, exit", c->errstr);
        kill(getpid(), SIGTERM);
    }
    log_info("%s", "redis connect ok");

}

void WorkThread::redis_disconnect_callback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK){
        log_info("redis disconnect error:%s", c->errstr);
    }
    else{
        log_info("%s", "redis disconnect normal");
    }
    kill(getpid(), SIGTERM);
}

void WorkThread::redis_login_callback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = (redisReply *)r;
    LoginData *login_data = (LoginData *)privdata;
    WorkThread *work_thread = (WorkThread *)login_data->work_thread;
    
    if (reply == NULL || reply->str == NULL){  //redis 出错或用户名不存在
        log_info("login redis check error. id:%lu, token:%s", login_data->id, login_data->token.c_str());
        log_debug("%s", "need http check");
        return ;
    }
    else{
        int rsp_len = 0;
        unsigned char *rsp_data = NULL;
        if (strcmp(login_data->token.c_str(), reply->str) == 0)
        {
            //当前线程已存在该id，报错。此检测不能检测到其他线程重复id，服务器可能同时存在相同id登录，落在不同线程。
            if (work_thread->_sub_map.find(login_data->id) != work_thread->_sub_map.end()){
                log_info("id already exist. id:%ld", login_data->id);
                rsp_data = work_thread->_analyse_req.get_login_rsp(false, std::string("id already exist!"), 
                            work_thread->_ping_time, &rsp_len);
            }
            else{  //成功登陆
                rsp_data = work_thread->_analyse_req.get_login_rsp(true, std::string("200 ok"), 
                            work_thread->_ping_time, &rsp_len);
                work_thread->_bev_id.insert(std::make_pair<intptr_t, uint64_t>(login_data->bev, login_data->id));
                Subscriber *sub = new Subscriber;
                sub->set_id(login_data->id);
                work_thread->_sub_map.insert(std::make_pair<uint64_t, Subscriber *>(login_data->id, sub));
                log_info("login success. id:%ld", login_data->id);
            }
        }
        else{  //验证失败
            rsp_data = work_thread->_analyse_req.get_login_rsp(false, std::string("wrong id or token"), 
                        work_thread->_ping_time, &rsp_len);
            log_info("login fail. id:%ld", login_data->id);
        }

        //int n = bufferevent_write((struct bufferevent *)login_data->bev, rsp_data, rsp_len);
        struct evbuffer *output = bufferevent_get_output((struct bufferevent *)login_data->bev);
        if (evbuffer_add(output, rsp_data, rsp_len) != 0){
            log_info("%s", "write login reply msg error.");
        }
        delete []rsp_data;
        delete login_data;
    }
}
