#include "handle_account.h"

#include <stdio.h>

HandleAccount::HandleAccount()
{}

HandleAccount::~HandleAccount()
{}

int HandleAccount::get_login_req_data(const int64_t id, const std::string &token, char *data)
{
    _check_id_req.Clear();
    _serialize_str.clear();
    _check_id_req.set_id(id);
    _check_id_req.set_token(token);
    _check_id_req.SerializePartialToString(&_serialize_str);

    return pack_req(data);
}

int HandleAccount::pack_req(char *data)
{
    _req.Clear();
    _req.set_mask(0);
    _req.set_udi("ei=1&ai=1&bd=pada");
    _req.set_reqno(0);
    _req.set_rsakeyver("1");
    _req.set_clientid(111);
    _req.add_action("ReqCheckTokenByID");
    _req.add_params(_serialize_str.c_str());
    _serialize_str.clear();
    _req.SerializePartialToString(&_serialize_str);

    const char *http_post = "POST / HTTP/1.1\r\n"
                            "Host:%s\r\n"
                            //"Content-Type: application/x-www-form-urlencoded\r\n"
                            "Content-Length:%d\r\n\r\n"
                            //"Connection:keep-alive\r\n"
                            //"Accept:*/*\r\n"
                            //"Accept-Encoding:gzip, deflate\r\n\r\n"
                            ;
    sprintf(data, http_post, HOST_URL, _req.ByteSize());
    int len = strlen(data);
    memcpy(data+len, _serialize_str.c_str(), _serialize_str.size());
    return len+_serialize_str.size();
}

bool HandleAccount::analyse_login_data(char *data, int len)
{
    if (!analyse_req(data, len))
      return false;

   _check_id_rsp.ParseFromString(_rsp.params(0));
   printf("%s\n", _check_id_rsp.resmsg().c_str());
   return _check_id_rsp.rescode() == 0;
}
        
bool HandleAccount::analyse_req(char *data, int len)
{
    int end_pos = len;
    int start_pos = end_pos - 1;
    while (data[start_pos] != 0x0a && start_pos > 0) start_pos--;

    _rsp.ParseFromArray(data + start_pos + 1, end_pos - start_pos - 1);
    if (_rsp.params_size() == 0)
    {
        return false;
    }
    return true;
}

int HandleAccount::get_relation_data(const int64_t from_id, const int64_t to_id, char *data)
{
    _check_relation_req.Clear();
    _check_relation_req.set_fromid(from_id);
    _check_relation_req.set_toid(to_id);
    _check_relation_req.SerializePartialToString(&_serialize_str);

    return pack_req(data);
}

bool HandleAccount::analyse_relation_data(char *data, int len)
{
    if (!analyse_req(data, len))
      return false;

    _check_relation_rsp.ParseFromString(_rsp.params(0));
    return _check_relation_rsp.rescode() == 0;
}
