#ifndef _HANDLE_ACCOUNT_H_
#define _HANDLE_ACCOUNT_H_

#include "comet.pb.h"

#include <string>

#define HOST_URL "uac3.api.stage.eaglenet.cn"


class HandleAccount
{
    public:
        HandleAccount();
        ~HandleAccount();

        //将数据组合成http post请求数据，data需有足够的空间。
        int get_login_req_data(const int64_t id, const std::string &token, char *data);
        //解析收到的账号验证数据
        bool analyse_login_data(char *data, int len);

        //生成账号关系验证http请求数据
        int get_relation_data(const int64_t from_id, const int64_t to_id, char *data);
        //解析收到的账号关系验证数据
        bool analyse_relation_data(char *data, int len);
    
    protected:
        //打包最外面的请求包
        int pack_req(char *data);
        //解析最外面的应答包
        bool analyse_req(char *data, int len);
    
    private:
        eglcomet::ReqPacket _req;
        eglcomet::RspPacket _rsp;

        eglcomet::ReqCheckTokenByID _check_id_req;
        eglcomet::RspCheckTokenByID _check_id_rsp;

        eglcomet::ReqCheckRelationByID _check_relation_req;
        eglcomet::RspCheckRelationByID _check_relation_rsp;

        std::string _serialize_str;
};

#endif  //_HANDLE_ACCOUNT_H_
