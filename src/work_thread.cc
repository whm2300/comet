#include "work_thread.h"
#include "log.h"
#include "shared_data.h"
#include "subscriber.h"

#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <event2/bufferevent.h>
#include <event2/buffer.h>

WorkThread::WorkThread():_evbase(NULL), _work_num(0), _conn_num(0),_redis(NULL)
{}

WorkThread::~WorkThread()
{}

bool WorkThread::init_work(const int work_num, PthreadInfo *work_thread, const int work_thread_num, 
                           const int max_num_per_thread, const int ping_time, const std::string &http_url, const std::string &http_ip)
{
    if (pipe(_new_conn_fd) != 0){
        log_error("%s", "create pipe file description error");
        return false;
    }
    if (!setnonblock(_new_conn_fd[0]) || !setnonblock(_new_conn_fd[1])){
        log_error("%s", "set new_conn_fd nonblock error");
        return false;
    }
    if (pipe(_new_msg_fd) != 0){
        log_error("%s", "create socket pair error");
        return false;
    }
    if (!setnonblock(_new_msg_fd[0]) || !setnonblock(_new_msg_fd[1])){
        log_error("%s", "set new_msg_fd nonblock error");
        return false;
    }
    _work_num = work_num;
    _work_thread = work_thread;
    _work_thread_num = work_thread_num;
    _ping_time = ping_time;
    _sub_map.rehash(max_num_per_thread * 1.2);
    _bev_id.rehash(max_num_per_thread * 1.2);
    _http_url = http_url;
    _http_ip = http_ip;

    _evbase = event_base_new();
    if (_evbase == NULL){
        log_error("%s", "Could not create evbase");
        return false;
    }

    struct event *notify = event_new(_evbase, _new_conn_fd[0], EV_READ | EV_PERSIST, notify_new_conn_callback, this);
    if (notify == NULL){
        log_error("%s", "Could not create notify new connection event");
        return false;
    }
    if (event_add(notify, NULL) != 0){
        log_error("%s, %d", "add notify new connection event error", work_num);
        return false;
    }

    notify = event_new(_evbase, _new_msg_fd[0], EV_READ | EV_PERSIST, notify_new_msg_callback, this);
    if (notify == NULL){
        log_error("%s", "Could not create notify new msg event");
        return false;
    }
    if (event_add(notify, NULL) != 0){
        log_error("%s, %d", "add notify new msg event error", work_num);
    }

    return true;
}

bool WorkThread::setnonblock(int fd)
{
    int fdflags;
    if ((fdflags = fcntl(fd, F_GETFL, 0)) == -1){
        return false;
    }
    fdflags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, fdflags) == -1){
        return false;
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

    log_debug("in notify_new_conn_callback");
    while (read(fd, &data, sizeof(data)) == sizeof(data)){
        log_debug("new conn:%d", data);
        if (data > 0){  // new connection
            struct bufferevent *bev = bufferevent_socket_new(work_thread->_evbase, data, BEV_OPT_CLOSE_ON_FREE);
            bufferevent_setcb(bev, bufferevent_read_callback, NULL, bufferevent_event_callback, user_data);
            bufferevent_enable(bev, EV_READ | EV_WRITE);
            work_thread->_bev_id.insert(std::make_pair<intptr_t, uint64_t>((intptr_t)bev, 0));
            __sync_fetch_and_add(&work_thread->_conn_num, 1);
        }
        else{  //cmd
            if (data == -1){  //stop cmd
                event_base_loopbreak(work_thread->_evbase);
                break;
            }
            else if (data == -2){  //timeout cmd
                work_thread->del_expire_client();
            }
        }
    }
    log_debug("out notify_new_conn_callback");
}

bool WorkThread::notify_new_msg(intptr_t ptr)
{
    TransData *trans_data = (TransData *)ptr;

    SubMap::iterator pos = _sub_map.find(trans_data->to_id);
    if (pos != _sub_map.end()){
        int res = -1;
        do{
            res = write(_new_msg_fd[1], &ptr, sizeof(ptr));
        }while(res == EAGAIN);
        if (res == sizeof(ptr)){
            log_debug("send new message success. id:%ld", trans_data->to_id);
            return true;
        }
        else{
            log_debug("send new message fail, write error. id:%ld", trans_data->to_id);
            return false;
        }
    }
    else{
        log_debug("send new message fail, not find sub. id:%ld", trans_data->to_id);
        return false;
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
                uint64_t id = id_pos->first;
                log_info("id:%ld, timeout", id_pos->first);
                delete id_pos->second;
                bufferevent_free((struct bufferevent *)bev_pos->first);
                bev_pos = _bev_id.erase(bev_pos);
                __sync_fetch_and_sub(&_conn_num, 1);
                id_pos = _sub_map.erase(id_pos);
                if (id != 0){
                    OfflineInfo *offline_info = new OfflineInfo;
                    offline_info->id = id;
                    offline_info->work_thread = this;
                    redisAsyncCommand(_redis, redis_offline_notify, 
                                (char*)offline_info, "SMEMBERS sUD:%ld", offline_info->id);
                }
            }
            else{
                ++bev_pos;
            }
        }
        else{
            ++bev_pos;
        }
    }

    //statistical data
    log_info("current bev client size:%d, current id client size:%d", _bev_id.size(), _sub_map.size());

    //test
    //SubMap::iterator pos = _sub_map.begin();
    //while (pos != _sub_map.end()){
    //    log_info("online id:%ld", pos->first);
    //    ++ pos;
    //}
}

void WorkThread::notify_new_msg_callback(evutil_socket_t fd, short event, void *user_data)
{
    WorkThread *work_thread = (WorkThread *)user_data;
    TransData *trans_data = NULL;
    while (read(fd, &trans_data, sizeof(trans_data)) == sizeof(trans_data)){
        SubMap::iterator pos = work_thread->_sub_map.find(trans_data->to_id);
        if (pos != work_thread->_sub_map.end()){
            struct evbuffer *output = bufferevent_get_output(pos->second->get_bev());
            if (output != NULL && evbuffer_add(output, trans_data->data, trans_data->data_size) == 0){
                log_debug("trans msg ok. to id:%ld", trans_data->to_id);
            }
        }
        delete []trans_data;
        trans_data = NULL;
    }
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

        //login
        LoginData *login_data = work_thread->_analyse_req.get_login_data();
        if (login_data != NULL){
            login_data->work_thread = (intptr_t)work_thread;
            login_data->bev = (intptr_t)bev;
            redisAsyncCommand(work_thread->_redis, redis_login_callback,
                        (char*)login_data, "GET sToken:%ld", login_data->id&0x0000FFFFFFFFFFFF);
            log_debug("%s, bev:%lx", "get login data, start check.", bev);

        }

        //heartbeat
        if (work_thread->_analyse_req.is_heart() && id != 0){
            SubMap::iterator pos = work_thread->_sub_map.find(id);
            if (pos != work_thread->_sub_map.end()){
                pos->second->update_heartbeat();
                log_debug("%s", "update hearbeat time");
            }
        }

        //trans
        TransData *trans_data = work_thread->_analyse_req.get_trans_data();
        if (trans_data != NULL){
            trans_data->work_thread = (intptr_t)work_thread;
            //if (work_thread->_sub_map.find(trans_data->from_id) != work_thread->_sub_map.end()){
            if (id != 0 && id == trans_data->from_id){
                log_debug("start transmit message, from_id:%ld to_id:%ld", trans_data->from_id, trans_data->to_id);
                if ((id & 0xFFFF000000000000) != 0){  //微信发送，不验证好友关系，直接转发。
                    if (!work_thread->send_message(trans_data)){
                        delete []trans_data;
                    }
                }
                else{
                    redisAsyncCommand(work_thread->_redis, redis_trans_callback, 
                                (char*)trans_data, "SMEMBERS sUD:%ld", trans_data->from_id);
                }
            }
            else{  //not login
                log_debug("find from id in map error, not login. from id:%ld, to id:%ld", trans_data->from_id, trans_data->to_id);
                delete []trans_data;
            }
        }
    }
}

void WorkThread::bufferevent_event_callback(struct bufferevent *bev, short events, void *user_data)
{
    WorkThread *work_thread = (WorkThread *)user_data;
    log_debug("in buffer_event_callback");

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

        struct evbuffer *input = bufferevent_get_input(bev);
        struct evbuffer *output = bufferevent_get_output(bev);
        size_t input_len = evbuffer_get_length(input);
        size_t output_len = evbuffer_get_length(output);
        if (events & BEV_EVENT_EOF){
            log_info("get a close from %ld, we drained %d bytes from input, %d from output.", id, input_len, output_len);
        }
        else{
            log_info("get a close from %ld, we drained %d bytes from input, %d from output.", id, input_len, output_len);
        }

        __sync_fetch_and_sub(&work_thread->_conn_num, 1);
        bufferevent_free(bev);

        //发送下线通知
        if (id != 0){
            OfflineInfo *offline_info = new OfflineInfo;
            offline_info->id = id;
            offline_info->work_thread = work_thread;
            redisAsyncCommand(work_thread->_redis, redis_offline_notify, 
                        (char*)offline_info, "SMEMBERS sUD:%ld", offline_info->id);
        }
    }
}

void *WorkThread::run(void *arg)
{
    WorkThread *work = (WorkThread *)arg;
    ThreadInfo *thread_info = new ThreadInfo;
    thread_info->thread_id = work->_work_num;
    thread_info->evbase = work->_evbase;

    if (pthread_setspecific(SharedData::get_instance()->get_pthread_key(), thread_info) != 0){
        log_error("id:%d, set thread private data error. %s, thread exit!", work->_work_num, strerror(errno));
        return NULL;
    }

    log_info("%s", "work thread start loop");
    int res = event_base_dispatch(work->_evbase);
    log_info("%s. exit code:%d", "work thread exit loop", res);
    delete thread_info;

    return NULL;
}

void WorkThread::free_data()
{
    event_base_free(_evbase);
    /*need fix 退出程序应释放内存，但执行会coredump，暂不释放，影响不大。
    SubMap::iterator it = _sub_map.begin();
    while (it != _sub_map.end()){
        delete it->second;
    }
    */
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

    if (work_thread->_bev_id.find(login_data->bev) == work_thread->_bev_id.end()){
        log_debug("disconnect id:%ld, stop redis check", login_data->id);
        delete login_data;
        return ;
    }

    if (reply == NULL || reply->str == NULL){  //redis 出错或用户名不存在
        log_info("login redis check error. start http check. id:%lu, token:%s", login_data->id, login_data->token.c_str());
        if (!work_thread->http_login_check(login_data)){
            delete login_data;
        }
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
                work_thread->_bev_id[login_data->bev] = login_data->id;
                Subscriber *sub = new Subscriber;
                sub->set_id_bev(login_data->id, (struct bufferevent *)login_data->bev);
                work_thread->_sub_map.insert(std::make_pair<uint64_t, Subscriber *>(login_data->id, sub));
                log_info("redis login success. id:%ld", login_data->id);

                //发送好友音箱在线状态
                OnlineInfo *online_info = new OnlineInfo;
                online_info->bev = (struct bufferevent *)login_data->bev;
                online_info->work_thread = work_thread;
                online_info->id = login_data->id;
                redisAsyncCommand(work_thread->_redis, redis_online_notify, 
                            (char*)online_info, "SMEMBERS sUD:%ld", login_data->id);

            }
        }
        else{  //验证失败
            rsp_data = work_thread->_analyse_req.get_login_rsp(false, std::string("wrong id or token"), 
                        work_thread->_ping_time, &rsp_len);
            log_info("redis login fail. id:%ld, token:%s", login_data->id, login_data->token.c_str());
        }

        struct evbuffer *output = bufferevent_get_output((struct bufferevent *)login_data->bev);
        if (output == NULL || evbuffer_add(output, rsp_data, rsp_len) != 0){
            log_error("%s", "write login reply msg error.");
        }
        delete []rsp_data;
        delete login_data;
    }
}

void WorkThread::redis_trans_callback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = (redisReply *)r;
    TransData *trans_data = (TransData *)privdata;
    WorkThread *work_thread = (WorkThread *)trans_data->work_thread;

    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0){
        log_debug("trans msg, redis check error, start http, from:%ld, to:%ld", trans_data->from_id, trans_data->to_id);
        if (!work_thread->http_trans_check(trans_data)){
            delete []trans_data;
        }
        return ;
    }
    else {
        char to_id_buffer[32];
        for (size_t i = 0; i < reply->elements; ++i){
            sprintf(to_id_buffer, "%ld", trans_data->to_id);
            if (strcmp(reply->element[i]->str, to_id_buffer) == 0){
                int j;
                for (j = 0; j < work_thread->_work_thread_num; ++j){
                    if (work_thread->_work_thread[j].work_thread->notify_new_msg((intptr_t)trans_data)){
                        log_debug("transmit message success. from_id:%ld to_id:%s", trans_data->from_id, reply->element[i]->str);
                        break;
                    }
                }
                if (j == work_thread->_work_thread_num){
                    log_debug("transmit message fail. to:%s", reply->element[i]->str);
                    delete []trans_data;
                }
                break;
            }
        }
    }
}

bool WorkThread::send_message(TransData *trans_data)
{
    int j;
    for (j = 0; j < _work_thread_num; ++j){
        if (_work_thread[j].work_thread->notify_new_msg((intptr_t)trans_data)){
            log_debug("transmit message success. from_id:%ld to_id:%ld", trans_data->from_id, trans_data->to_id);
            break;
        }
    }
    if (j == _work_thread_num){
        log_debug("transmit message fail. to_id:%ld", trans_data->to_id);
        return false;
    }
    return true;
}

void WorkThread::redis_online_notify(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = (redisReply *)r;
    OnlineInfo *online_info= (OnlineInfo *)privdata;
    WorkThread *work_thread = (WorkThread *)online_info->work_thread;

    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0){
        //redis中不存在任何好友列表，不做任何操作。
    }
    else if (work_thread->_bev_id.find((intptr_t)online_info->bev) != work_thread->_bev_id.end()){
        work_thread->_online_status.Clear();
        work_thread->_me_status.Clear();
        char id[32];
        memset(id, 0, 32);
        sprintf(id, "%ld", online_info->id);
        work_thread->_me_status.add_id(id);
        char on_line = 1;
        work_thread->_me_status.set_status((void *)&on_line, sizeof(on_line));

        int on_line_src_len = work_thread->_me_status.ByteSize();
        unsigned char on_line_src[on_line_src_len];
        work_thread->_me_status.SerializeToArray(on_line_src, on_line_src_len);
        int on_line_des_len = 0;
        unsigned char on_line_des[29 + on_line_des_len];
        work_thread->_analyse_req.pack_data(on_line_src, on_line_src_len, 12, on_line_des, on_line_des_len);

        char str_status[reply->elements];
        for (size_t i = 0; i < reply->elements; ++i){
            work_thread->_online_status.add_id(reply->element[i]->str);
            int j;
            for (j = 0; j < work_thread->_work_thread_num; ++j){
                uint64_t to_id = atoll(reply->element[i]->str);
                if (work_thread->_work_thread[j].work_thread->check_id_is_online(to_id)){
                    str_status[i] = 1;

                    //存在已经在线的好友音箱，发送当前设备已经登录信息。
                    TransData *trans_data = (TransData *) new char[sizeof(TransData) + on_line_des_len];
                    trans_data->from_id = online_info->id;
                    trans_data->to_id = to_id;
                    trans_data->data_size = on_line_des_len;
                    memcpy(trans_data->data, on_line_des, on_line_des_len);
                    if (!work_thread->_work_thread[j].work_thread->notify_new_msg((intptr_t)trans_data)){
                        log_error("send online notify error, to id:%ld", to_id);
                        delete []trans_data;
                    }
                    break;
                }
            }
            if (j == work_thread->_work_thread_num){
                str_status[i] = 0;
            }
        }
        //向当前设备下发好友列表中已经登录的设备
        work_thread->_online_status.set_status((void *)str_status, reply->elements);
        int src_len = work_thread->_online_status.ByteSize();
        unsigned char src[src_len];
        work_thread->_online_status.SerializeToArray(src, src_len);
        int des_len = 0;
        unsigned char des[29 + src_len];
        work_thread->_analyse_req.pack_data(src, src_len, 12, des, des_len);
        log_debug("src size:%d, des size:%d", src_len, des_len);

        struct evbuffer *output = bufferevent_get_output(online_info->bev);
        evbuffer_add(output, des, des_len);
    }

    delete online_info;
}

void WorkThread::redis_offline_notify(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = (redisReply *)r;
    OfflineInfo *offline_info= (OfflineInfo *)privdata;
    WorkThread *work_thread = (WorkThread *)offline_info->work_thread;

    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements == 0){
        //redis中不存在任何好友列表，不做任何操作。
    }
    else{
        work_thread->_me_status.Clear();
        char id[32];
        memset(id, 0, 32);
        sprintf(id, "%ld", offline_info->id);
        work_thread->_me_status.add_id(id);
        char on_line = 0;
        work_thread->_me_status.set_status((void *)&on_line, sizeof(on_line));

        int off_line_src_len = work_thread->_me_status.ByteSize();
        unsigned char off_line_src[off_line_src_len];
        work_thread->_me_status.SerializeToArray(off_line_src, off_line_src_len);
        int off_line_des_len = 0;
        unsigned char off_line_des[29 + off_line_des_len];
        work_thread->_analyse_req.pack_data(off_line_src, off_line_src_len, 12, off_line_des, off_line_des_len);

        for (size_t i = 0; i < reply->elements; ++i){
            for (int j = 0; j < work_thread->_work_thread_num; ++j){
                uint64_t to_id = atoll(reply->element[i]->str);
                if (work_thread->_work_thread[j].work_thread->check_id_is_online(to_id)){
                    //存在已经在线的好友音箱，发送当前设备下线信息。
                    TransData *trans_data = (TransData *) new char[sizeof(TransData) + off_line_des_len];
                    trans_data->from_id = offline_info->id;
                    trans_data->to_id = to_id;
                    trans_data->data_size = off_line_des_len;
                    memcpy(trans_data->data, off_line_des, off_line_des_len);
                    if (!work_thread->_work_thread[j].work_thread->notify_new_msg((intptr_t)trans_data)){
                        log_error("send offline msg error to id:%ld", to_id);
                    }
                    break;
                }
            }
        }
    }
    delete offline_info;
}

bool WorkThread::http_login_check(LoginData *login_data)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(_http_ip.c_str());

    struct bufferevent *bev = bufferevent_socket_new(_evbase, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, http_login_read, http_login_write, http_login_event, login_data);
    if (bufferevent_socket_connect(bev, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        log_error("connect http server error. id:%ld", login_data->id);
        bufferevent_free(bev);
        return false;
    }
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return true;
}

void WorkThread::http_login_read(struct bufferevent *bev, void *ctx)
{
    //没有做数据是否完全达到判断，可能存在bug
    struct evbuffer *input = bufferevent_get_input(bev);
    int buffer_data_size = evbuffer_get_length(input);
    char data[buffer_data_size + 1];
    evbuffer_remove(input, data, buffer_data_size);
    data[buffer_data_size] = '\0';
    LoginData *login_data = (LoginData *)ctx;
    WorkThread *work_thread = (WorkThread *)login_data->work_thread;
    log_debug("%s", data);

    //客户端连接还有效
    if (work_thread->_bev_id.find(login_data->bev) != work_thread->_bev_id.end()){
        bool r = work_thread->_analyse_req.analyse_login_data(data, buffer_data_size);
        int rsp_len = 0;
        unsigned char *rsp_data = NULL;
        if (r){
            rsp_data = work_thread->_analyse_req.get_login_rsp(true, std::string("200 ok"), 
                        work_thread->_ping_time, &rsp_len);
            work_thread->_bev_id[login_data->bev] = login_data->id;
            Subscriber *sub = new Subscriber;
            sub->set_id_bev(login_data->id, (struct bufferevent *)login_data->bev);
            work_thread->_sub_map.insert(std::make_pair<uint64_t, Subscriber *>(login_data->id, sub));
            log_info("http login success. id:%ld", login_data->id);

            //发送好友音箱在线状态
            OnlineInfo *online_info = new OnlineInfo;
            online_info->bev = (struct bufferevent *)login_data->bev;
            online_info->work_thread = work_thread;
            online_info->id = login_data->id;
            redisAsyncCommand(work_thread->_redis, redis_online_notify, 
                        (char*)online_info, "SMEMBERS sUD:%ld", login_data->id);

        }
        else{  //验证失败
            rsp_data = work_thread->_analyse_req.get_login_rsp(false, std::string("wrong id or token"), 
                        work_thread->_ping_time, &rsp_len);
            log_info("http login fail. id:%ld", login_data->id);
        }

        struct evbuffer *output = bufferevent_get_output((struct bufferevent *)login_data->bev);
        if (evbuffer_add(output, rsp_data, rsp_len) != 0){
            log_info("%s", "write login reply msg error.");
        }
        delete []rsp_data;
    }
    else{
        log_error("clinet disconnect. id:%ld", login_data->id);
    }
    log_debug("out http_login_read");
    bufferevent_free(bev);
    delete login_data;
}

void WorkThread::http_login_write(struct bufferevent *bev, void *ctx)
{
    log_debug("in http_login_write");
    /*
    LoginData *login_data = (LoginData *)ctx;
    WorkThread *work_thread = (WorkThread *)login_data->work_thread;

    char buffer[1024];
    int len = work_thread->_analyse_req.get_login_post_data(login_data->id, login_data->token, 
                work_thread->_http_url, buffer);
    struct evbuffer *output = bufferevent_get_output(bev);
    if (output == NULL || (evbuffer_add(output, buffer, len) != 0)){
        log_error("send http check error. id:%ld", login_data->id);
        bufferevent_free(bev);
        delete login_data;
    }
    log_debug("send http check ok");
    */
}

void WorkThread::http_login_event(struct bufferevent *bev, short event, void *ctx)
{
    //http 连接错误
    LoginData *login_data = (LoginData *)ctx;
    WorkThread *work_thread = (WorkThread *)login_data->work_thread;
    if (event & (BEV_EVENT_EOF | BEV_EVENT_ERROR)){
        log_error("http check error. id:%ld", login_data->id);;
        //如果客户端连接还有效，下发http验证问题
        if (work_thread->_bev_id.find(login_data->id) != work_thread->_bev_id.end()){
            int rsp_len = 0;
            unsigned char *rsp_data = NULL;
            rsp_data = work_thread->_analyse_req.get_login_rsp(false, std::string("http server error"), 
                        work_thread->_ping_time, &rsp_len);
            struct evbuffer *output = bufferevent_get_output((struct bufferevent *)login_data->bev);
            if (evbuffer_add(output, rsp_data, rsp_len) != 0){
                log_error("reply http error. id:%ld", login_data->id);
            }
            delete rsp_data;
        }
        delete login_data;
        bufferevent_free(bev);
    }
    else if(event & BEV_EVENT_CONNECTED){
        char buffer[1024];
        int len = work_thread->_analyse_req.get_login_post_data(login_data->id, login_data->token, 
                    work_thread->_http_url, buffer);
        struct evbuffer *output = bufferevent_get_output(bev);
        if (output == NULL || (evbuffer_add(output, buffer, len) != 0)){
            log_error("send http check error. id:%ld", login_data->id);
            bufferevent_free(bev);
            delete login_data;
        }
        log_debug("send http check ok");
    }
    else{}
    log_debug("out http_login_event");
}

bool WorkThread::http_trans_check(TransData *trans_data)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(_http_ip.c_str());

    struct bufferevent *bev = bufferevent_socket_new(_evbase, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, http_trans_read, http_trans_write, http_trans_event, trans_data);
    if (bufferevent_socket_connect(bev, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        log_error("transmit data. connect http server error. ");
        bufferevent_free(bev);
        return false;
    }
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return true;
}

void WorkThread::http_trans_read(struct bufferevent *bev, void *ctx)
{
    //没有做数据是否完全达到判断，可能存在bug
    struct evbuffer *input = bufferevent_get_input(bev);
    int buffer_data_size = evbuffer_get_length(input);
    char data[buffer_data_size + 1];
    evbuffer_remove(input, data, buffer_data_size);
    data[buffer_data_size] = '\0';
    TransData *trans_data = (TransData *)ctx;
    WorkThread *work_thread = (WorkThread *)trans_data->work_thread;
    log_debug("%s", data);

    if (work_thread->_analyse_req.analyse_relation_data(data, buffer_data_size)){
        int j;
        for (j = 0; j < work_thread->_work_thread_num; ++j){
            if (work_thread->_work_thread[j].work_thread->notify_new_msg((intptr_t)trans_data)){
                log_debug("http transmit message success. to id:%ld", trans_data->to_id);
                break;
            }
        }
        if (j == work_thread->_work_thread_num){
            log_error("http transmit message fail. from id:%ld, to id:%ld", trans_data->from_id, trans_data->to_id);
            delete trans_data;
        }
    }
    else{
        log_error("http transmit message fail. from id:%ld, to id:%ld", trans_data->from_id, trans_data->to_id);
        delete trans_data;
    }

    bufferevent_free(bev);
}

void WorkThread::http_trans_write(struct bufferevent *bev, void *ctx)
{
    //log_debug("http for transmit data. ready to write");
}

void WorkThread::http_trans_event(struct bufferevent *bev, short event, void *ctx)
{
    TransData *trans_data = (TransData *)ctx;
    WorkThread *work_thread = (WorkThread *)trans_data->work_thread;
    if (event & (BEV_EVENT_EOF | BEV_EVENT_ERROR)){  //http中断错误
        log_error("http disconnect when transmit data");
        delete []trans_data;
        bufferevent_free(bev);
    }
    else if(event & BEV_EVENT_CONNECTED){  //http可以写
        char buffer[1024];
        int len = work_thread->_analyse_req.get_relation_post_data(trans_data->from_id, trans_data->to_id, 
                    work_thread->_http_url, buffer);
        struct evbuffer *output = bufferevent_get_output(bev);
        if (output == NULL || (evbuffer_add(output, buffer, len) != 0)){
            log_error("send http check error when transmit data. from id:%ld, to id:%ld", trans_data->from_id, trans_data->to_id);
            bufferevent_free(bev);
            delete []trans_data;
        }
        log_debug("send http check ok");
    }
    else{}
}
