#include "analyse_req.h"
#include "log.h"

#include <event2/buffer.h>

AnalyseReq::AnalyseReq():_login_data(NULL), _trans_data(NULL)
{}

AnalyseReq::~AnalyseReq()
{}

void AnalyseReq::init_data()
{
}

bool AnalyseReq::analyse_data(struct evbuffer *input, struct evbuffer *output)
{
    init_data();
    int buffer_size = evbuffer_get_length(input);
    unsigned char *data = evbuffer_pullup(input, 2);
    if (data == NULL){
        return false;
    }
    int head_len = (data[0]&0x0F)*4;

    log_debug("get data. buffer size:%ld, head size:%ld", buffer_size, head_len);
    if (data[1]&0x20){  //心跳包
        _is_heart = true;
        evbuffer_remove_buffer(input, output, 4);
        log_debug("receive heartbeat bag.");
        return true;
    }

    if (buffer_size > head_len + 5){  //数据中的第一个长度字段已到达
        int32_t data_len = 0;
        int32_t data_head_len = 0;
        data = evbuffer_pullup(input, head_len + 5);
        if (data == NULL){
            return false;
        }
        if (data[head_len+3] == 0xFF && data[head_len+4] == 0x0){  //数据长度放在之后的四个字节
            if (buffer_size > head_len + 9){
                data = evbuffer_pullup(input, head_len + 9);
                if (data == NULL){
                    return false;
                }
                uint32_t *p = (uint32_t *)(data + head_len + 5);
                data_len = ntohl(*p);
                data_head_len = 9;
            }
            else{  //数据未完全到达
                return false;
            }
        }
        else{
            uint16_t *p = (uint16_t *)(data + head_len + 3);
            data_len = (uint32_t)ntohs(*p);
            data_head_len = 5;
        }

        log_debug("buffer_size:%d, head_len:%d, data_head_len:%d, data_len:%d", buffer_size, head_len, data_head_len, data_len);
        if (buffer_size >= head_len + data_head_len + data_len){
            return analyse_pack(input, head_len, data_head_len, data_len);
        }
        else{  //数据未完全到达
            return false;
        }
    }

    return false;  //数据未完全到达
}

bool AnalyseReq::analyse_pack(struct evbuffer *input, const int head_len, 
                              const int data_head_len, const int data_len)
{
    int pack_size = head_len + data_head_len + data_len;
    unsigned char *pack_bag = evbuffer_pullup(input, pack_size);
    
    if (pack_bag == NULL){
        return false;
    }

    if (pack_bag[1]&0x40){  //转发包
        log_debug("%s", "get trans data");
        _trans_data = (TransData *) new unsigned char[sizeof(TransData) + pack_size];
        unsigned char *p = (unsigned char *)&_trans_data->to_id;
        for (int i = 7; i > -1;){
            *p++ = (pack_bag + 12)[i--];
        }
        p = (unsigned char *)&_trans_data->from_id;
        for (int i = 7; i > -1;){
            *p++ = (pack_bag + 4)[i--];
        }
        log_debug("analyse_pack, from_id:%ld, to_id:%ld", _trans_data->from_id, _trans_data->to_id);
        _trans_data->data_size = pack_size;
        evbuffer_remove(input, _trans_data->data, pack_size);
    }
    else{
        uint16_t property_id = pack_bag[head_len]<<8 | pack_bag[head_len + 1];
        log_debug("property id:%d", property_id);
        switch (property_id)
        {
            case 1 :
                {
                    log_error("%s", "wechat bag not handle");
                    evbuffer_drain(input, head_len+data_head_len+data_len);
                }
                break;

            case 10 :
                {
                    if (!analyse_login_pack(pack_bag+head_len+data_head_len, data_len)){
                        return false;
                    }
                    evbuffer_drain(input, head_len+data_head_len+data_len);
                }
                break;

            case 12 :
                log_error("%s", "not handle check yet");
                evbuffer_drain(input, head_len+data_head_len+data_len);
                break;

            default :
                log_error("%s", "package error, drop data.");
                evbuffer_drain(input, head_len+data_head_len+data_len);
                return false;
        }
    }
    return true;
}

bool AnalyseReq::analyse_login_pack(const unsigned char *data, const int data_size)
{
    _req_login.ParseFromArray(data, data_size);

    _login_data = new LoginData;
    _login_data->id = _req_login.id();
    _login_data->token = _req_login.token();

    return true;
}

unsigned char *AnalyseReq::get_login_rsp(const bool is_success, const std::string &msg, const int ping_time, int *rsp_len)
{
    if (is_success){
        _rsp_login.set_code(200);
    }
    else{
        _rsp_login.set_code(401);
    }
    _rsp_login.set_msg(msg);
    _rsp_login.set_ping_time(ping_time);

    int data_size = _rsp_login.ByteSize();
    unsigned char *rsp_data = NULL;
    if (data_size > 0xFF00){
        *rsp_len = 20 + 9 + data_size;
        rsp_data = new unsigned char[*rsp_len];
        rsp_data[23] = 0xFF;
        rsp_data[24] = 0x00;
        uint32_t net_size = htonl(data_size);
        memcpy(rsp_data+25, &net_size, 4);
        _rsp_login.SerializeToArray(rsp_data+29, data_size);
    }
    else{
        *rsp_len = 20 + 5 + data_size;
        rsp_data = new unsigned char[*rsp_len];
        uint16_t net_size = htons((uint16_t)data_size);
        memcpy(rsp_data+23, &net_size, 2);
        _rsp_login.SerializeToArray(rsp_data+25, data_size);
    }
    rsp_data[0] = 0x10|(20/4);  //版本和头部长度
    rsp_data[1] = 0x0;  //NALU
    rsp_data[2] = 0x0;  //TTL
    rsp_data[3] = 0x0;  //RES
    int16_t net_id = htons(11);
    memcpy(rsp_data+20, &net_id, 2);
    memset(rsp_data+4, 0, 8);
    memset(rsp_data+12, 0, 8);
    char format = 0x01;
    memcpy(rsp_data+22, &format, 1);
    return rsp_data;
}

bool AnalyseReq::pack_data(unsigned char *src, const int src_len, 
                           const int property_id, unsigned char *des, int &des_len)
{
    if (src_len > 0xFF00){
        des_len = 20 + 9 + src_len;
        des[23] = 0xFF;
        des[24] = 0x00;
        uint32_t net_size = htonl(src_len);
        memcpy(des+25, &net_size, 4);
        memcpy(des+29, src, src_len);
    }
    else{
        des_len = 20 + 5 + src_len;
        uint16_t net_size = htons(src_len);
        memcpy(des+23, &net_size, sizeof(net_size));
        memcpy(des+25, src, src_len);
    }
    des[0] = 0x10 | (20/4);
    des[1] = 0x0;
    des[3] = 0x0;
    int16_t net_id = htons(12);
    memcpy(des+20, &net_id, 2);
    char format = 0x01;
    memcpy(des+22, &format, 1);
    memset(des+4, 0, 8);
    memset(des+12, 0, 8);
    return true;
}

LoginData *AnalyseReq::get_login_data()
{
    LoginData *p = _login_data;
    _login_data = NULL;
    return p;
}
TransData *AnalyseReq::get_trans_data()
{
    TransData *p = _trans_data;
    _trans_data = NULL;
    return p;
}

bool AnalyseReq::is_heart()
{
    bool r = _is_heart;
    _is_heart= false;
    return r;
}

int AnalyseReq::get_login_post_data(const int64_t id, const std::string &token, 
                                    const std::string &url, char *data)
{
    _check_id_req.Clear();
    _serialize_str.clear();
    _check_id_req.set_id(id);
    _check_id_req.set_token(token);
    _check_id_req.SerializePartialToString(&_serialize_str);

    return pack_post_data(data, (char *)"ReqCheckTokenByID", url);
}

bool AnalyseReq::analyse_login_data(char *data, int len)
{
    if (!analyse_post_data(data, len)){
        return false;
    }
    _check_id_rsp.ParseFromString(_rsp.params(0));
    log_debug("http login. %d, %s", _check_id_rsp.rescode(), _check_id_rsp.resmsg().c_str());
    return _check_id_rsp.rescode() == 0;
}

int AnalyseReq::get_relation_post_data(const int64_t from_id, const int64_t to_id, 
                                       const std::string &url, char *data)
{
    _serialize_str.clear();
    _check_relation_req.Clear();
    _check_relation_req.set_fromid(from_id);
    _check_relation_req.set_toid(to_id);
    _check_relation_req.SerializePartialToString(&_serialize_str);
    return pack_post_data(data, (char *)"ReqCheckRelationByID", url);
}

bool AnalyseReq::analyse_relation_data(char *data, int len)
{
    if (!analyse_post_data(data, len))
      return false;

    _check_relation_rsp.ParseFromString(_rsp.params(0));
    log_debug("http trans. %d, %s", _check_relation_rsp.rescode(), _check_relation_rsp.resmsg().c_str());
    return _check_relation_rsp.rescode() == 0;
}

int AnalyseReq::pack_post_data(char *data, char *action, const std::string &url)
{
    _req.Clear();
    _req.set_mask(0);
    _req.set_udi("ei=1&ai=1&bd=pada");
    _req.set_reqno(0);
    _req.set_rsakeyver("1");
    _req.set_clientid(111);
    _req.add_action(action);
    _req.add_params(_serialize_str.c_str());
    _serialize_str.clear();
    _req.SerializePartialToString(&_serialize_str);

    const char *http_post = "POST / HTTP/1.1\r\n"
                            "Host:%s\r\n"
                            "Content-Length:%d\r\n\r\n";
    sprintf(data, http_post, url.c_str(), _serialize_str.size());
    log_debug("%s", data);
    int len = strlen(data);
    memcpy(data+len, _serialize_str.c_str(), _serialize_str.size());
    return len+_serialize_str.size();
}

bool AnalyseReq::analyse_post_data(char *data, int len)
{
    int end_pos = len;
    int start_pos = end_pos - 1;
    while ((data[start_pos] != 0x0a || data[start_pos - 1] != 0x0d) && start_pos > 1) start_pos--;

    _rsp.ParseFromArray(data + start_pos + 1, end_pos - start_pos - 1);
    log_debug("%d, %s", _rsp.rescode(), _rsp.resmsg().c_str());
    if (_rsp.params_size() == 0){
        log_error("%s", "analyse response post data error. params size is 0.");
        return false;
    }
    return true;
}
