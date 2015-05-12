#include "mongo.hpp"
#include "log.h"
#include "comet.pb.h"

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <vector>

const std::string mongo_ip = "192.168.1.12";
const std::string mongo_port = "27017";

const std::string server_ip = "192.168.1.148";
const int         server_port[10] = {8888, 8889, 8890, 8891, 8892, 8893, 8894, 8895, 8896, 8897};

int         g_count = 60000;
const int         g_send_msg_sec = 1;
const int         g_heart_time = 30;

class ConnThread;
typedef struct tagClientInfo
{
    IntToStr::iterator pos;
    ConnThread         *conn;
    struct bufferevent *bev;
}ClientInfo;

typedef std::vector<ClientInfo *> ClientVector;

bool setnonblock(int fd)
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

class ConnThread
{
    public:
        ConnThread(){}
        ~ConnThread(){}

        bool init()
        {
            if (pipe(cmd_fd) != 0){
                log_error("create pipe fail");
                return false;
            }
            if (!setnonblock(cmd_fd[0]) || !setnonblock(cmd_fd[1])){
                log_error("set nonblock error");
                return false;
            }

            evbase = event_base_new();
            if (evbase == NULL){
                log_error("create evbase fail");
                return false;
            }
            struct event *notify = event_new(evbase, cmd_fd[0], EV_READ | EV_PERSIST, notify_callback, this);
            if (notify == NULL || event_add(notify, NULL) < 0){
                log_error("create notify event fail");
                return false;
            }
            struct event *heart = event_new(evbase, -1, EV_PERSIST, heart_callback, this);
            struct timeval tv;
            tv.tv_sec = g_heart_time;
            tv.tv_usec = 0;
            if (heart == NULL || evtimer_add(heart, &tv) < 0){
                log_error("create heart timer fail");
                return false;
            }
            
            struct event *send_msg = event_new(evbase, -1, EV_PERSIST, send_callback, this);
            tv.tv_sec = g_send_msg_sec;
            tv.tv_usec = 0;
            if (heart == NULL || evtimer_add(send_msg, &tv) < 0){
                log_error("create send timer fail");
                return false;
            }
            _client.reserve(g_count);

            return true;
        }

        bool notify(int cmd)
        {
            return write(cmd_fd[1], &cmd, sizeof(cmd)) == sizeof(cmd);
        }

        static void *run(void *arg)
        {
            ConnThread *conn = (ConnThread *)arg;
            event_base_dispatch(conn->evbase);
            log_info("thread exit");
            return NULL;
        }

        static void notify_callback(evutil_socket_t fd, short event, void *ctx)
        {
            ConnThread *conn = (ConnThread *)ctx;
            int data;
            if (read(fd, &data, sizeof(data)) != sizeof(data)){
                return ;
            }
            if (data == 1){  //stop cmd
                event_base_loopbreak(conn->evbase);
            }
            else if (data == 2){  //start connect
                struct sockaddr_in addr;
                memset(&addr, 0, sizeof(addr));
                addr.sin_family = AF_INET;
                addr.sin_port = htons(server_port[0]);
                addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

                IntToStr::iterator pos = conn->id_token.begin();
                while (pos != conn->id_token.end()){
                    struct bufferevent *bev = bufferevent_socket_new(conn->evbase, -1, BEV_OPT_CLOSE_ON_FREE);
                    ClientInfo *client_info = new ClientInfo;
                    client_info->pos = pos;
                    client_info->conn = conn;
                    client_info->bev = bev;
                    bufferevent_setcb(bev, read_callback, NULL, event_callback, client_info);
                    if (bufferevent_socket_connect(bev, (struct sockaddr *)&addr, sizeof(addr)) < 0){
                        log_error("connect http server error. %s", strerror(errno));
                        bufferevent_free(bev);
                        exit(-1);
                    }
                    bufferevent_enable(bev, EV_READ | EV_WRITE);
                    conn->_client.push_back(client_info);
                    ++pos;
                }
            }
        }

        static void heart_callback(evutil_socket_t sig, short events, void *ctx)
        {
            log_info("send heartbeat");
            ConnThread *conn = (ConnThread *)ctx;
            char heart_bag[4];
            heart_bag[0] = 0x11;
            heart_bag[1] = 0x20;
            memset(heart_bag+2, 0, 2);

            ClientVector::iterator pos = conn->_client.begin();
            while (pos != conn->_client.end()){
                struct evbuffer *output = bufferevent_get_output((*pos)->bev);
                evbuffer_add(output, heart_bag, 4);
                ++pos;
            }
        }


        static void read_callback(struct bufferevent *bev, void *ctx)
        {
            ClientInfo *client_info = (ClientInfo *)ctx;
            ConnThread *conn = (ConnThread *)client_info->conn;
            struct evbuffer *input = bufferevent_get_input(bev);
            size_t input_len = evbuffer_get_length(input);

            while (input_len > 3){
                unsigned char *data = evbuffer_pullup(input, 2);
                if (data[1]&0x20){
                    log_info("get heartbeat rsp.id:%ld", client_info->pos->first);
                    evbuffer_drain(input, 4);
                }
                else if (input_len > 25){
                    data = evbuffer_pullup(input, 25);
                    uint16_t *p = (uint16_t *)(data+23);
                    uint16_t len = (uint16_t)ntohs(*p);
                    if (input_len >= (size_t)len + 25){
                        data = evbuffer_pullup(input, 25+len);
                        uint16_t property_id = ntohs(*(uint16_t *)(data+20));
                        if (property_id == 11){
                            conn->_rsp_login.ParseFromArray(data+25, len);
                            if (conn->_rsp_login.code() == 200){
                                log_info("login ok. id:%ld", client_info->pos->first);
                            }
                            else{
                                log_info("login fail. id:%ld", client_info->pos->first);
                                //delete client_info;
                                //bufferevent_free(bev);
                                return;
                            }
                        }
                        else if (property_id == 12){
                            conn->_online.Clear();
                            conn->_online.ParseFromArray(data+25, len);
                            for (int i = 0; i < conn->_online.id_size(); i++){
                                log_info("src_id:%ld, friend_id:%s, status:%d", client_info->pos->first,  conn->_online.id(i).c_str(), conn->_online.status().c_str()[i]);
                            }
                            log_info("----------------");
                        }
                        else if (property_id == 22){
                            conn->_song.Clear();
                            conn->_song.ParseFromArray(data+25, len);
                            uint64_t from_id;
                            unsigned char *p = (unsigned char *)&from_id;
                            for (int i = 7; i > -1;){
                                *p++ = (data+4)[i--];
                            }
                            log_info("%ld->%ld, song info, id:%s, song_name:%s, source:%d, singers:%s, pic_url:%s", 
                                        from_id, client_info->pos->first, conn->_song.song_id().c_str(), conn->_song.song_name().c_str(), 
                                        conn->_song.source(), conn->_song.singers().c_str(), conn->_song.pic_url().c_str());
                        }
                        else{
                            log_error("property id not handle. %d", property_id);
                        }
                    }
                    evbuffer_drain(input, 25+len);
                }
                input_len = evbuffer_get_length(input);
            }
        }

        static void send_callback(evutil_socket_t sig, short events, void *ctx)
        {
            ConnThread *conn = (ConnThread *)ctx;

            char buffer[128];
            unsigned char data[1024];
            memset(data, 0, 1024);
            data[0] = 0x15;
            data[1] = 0x40;
            data[2] = 0x0;
            data[3] = 0x0;
            uint16_t property_id = htons(22);
            memcpy(data+20, &property_id, 2);
            data[22] = 0x01;

            time_t time;
            struct timeval tv;
            struct tm *tm;
            ClientVector::iterator pos = conn->_client.begin();
            for (; pos != conn->_client.end(); ++pos){
                uint64_t base_id = (*pos)->pos->first/5 * 5;
                for (int i = 0; i < 1; ++i){
                    uint64_t to_id = base_id + i;
                    if (to_id != (*pos)->pos->first){
                        sprintf(buffer, "%ld->%ld",(*pos)->pos->first, to_id);
                        conn->_song.Clear();
                        conn->_song.set_song_id(std::string(buffer));
                        conn->_song.set_song_name("apologize");
                        conn->_song.set_source(3);
                        conn->_song.set_singers("刘德华");
                        gettimeofday(&tv, NULL);
                        time = tv.tv_sec;
                        tm = ::localtime(&time);
                        sprintf(buffer, "%02d:%02d:%02d.%06d", tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec));
                        conn->_song.set_pic_url(buffer);

                        int data_len = conn->_song.ByteSize();
                        conn->_song.SerializeToArray(data+25, data_len);
                        uint16_t nlen = htons(data_len);
                        memcpy(data+23, &nlen, 2);
                        uint64_t id = (*pos)->pos->first;
                        unsigned char *p = (unsigned char *)&id;
                        for (int j = 7; j > -1;){
                            (data+4)[j--] = *p++;
                        }
                        id = to_id;
                        p = (unsigned char *)&id;
                        for (int j = 7; j > -1;){
                            (data+12)[j--] = *p++;
                        }

                        struct evbuffer *output = bufferevent_get_output((*pos)->bev);
                        evbuffer_add(output, data, 25+data_len);
                    }
                }
            }
        }

        static void event_callback(struct bufferevent *bev, short events, void *ctx)
        {
            ClientInfo *client_info = (ClientInfo *)ctx;
            ConnThread *conn = (ConnThread *)client_info->conn;
            if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)){
                log_error("server close connect. %ld", client_info->pos->first);
                delete client_info;
                bufferevent_free(bev);
            }
            else if (events & BEV_EVENT_CONNECTED){
                unsigned char buffer[512];
                int len = conn->get_login_data(client_info->pos->first, client_info->pos->second, buffer);
                struct evbuffer *output = bufferevent_get_output(bev);
                evbuffer_add(output, buffer, len);
                log_info("send login data over");
            }
        }

        int get_login_data(int64_t id, const std::string &token, unsigned char *data)
        {
            _req_login.set_id(id);
            _req_login.set_token(token);
            int log_size = _req_login.ByteSize();
            int data_size = 20 + 5 + log_size;
            uint16_t net_size = htons((uint16_t)log_size);
            memcpy(data + 23, &net_size, 2);
            _req_login.SerializeToArray(data + 25, log_size);

            data[0] = 0x10|(20/4);
            data[1] = 0x0;
            data[3] = 0x0;
            int16_t net_id = htons((uint16_t)10);
            memcpy(data+20, &net_id, 2);
            memset(data+4, 0, 8);
            memset(data+12, 0, 8);
            char format = 0x01;
            memcpy(data+22, &format, 1);
            return data_size;
        }

        int cmd_fd[2];
        struct event_base *evbase;
        IntToStr id_token;
        ClientVector _client;

        eglcomet::C2SLogin _req_login;
        eglcomet::S2CLogin _rsp_login;
        eglcomet::OnlineStatus _online;
        eglcomet::Song        _song;
};

int main(int argc, char *argv[])
{
    if (argc != 2){
        return -1;
    }
    g_count = atoi(argv[1]);

    SingletonLog::get_instance()->open_log(std::string("./logs/log.txt"), std::string("debug"));

    ConnThread *conn_a = new ConnThread;
    conn_a->init();

    Mongo a;
    a.open_db(mongo_ip, mongo_port);
    a.get_id_token("comet.account_a", conn_a->id_token, g_count);

    pthread_t aid;
    pthread_create(&aid, NULL, ConnThread::run, conn_a);

    conn_a->notify(2);

    pthread_join(aid, NULL);

    return 0;
}
