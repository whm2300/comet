/*=============================================================================
#      Filename : analyse_req.h
#   Description : 
#        Author : chenqingming chenqingming0710@163.com
#        create : 2015-03-25 13:57
# Last modified : 2015-03-25 13:57
=============================================================================*/
#ifndef _ANALYSE_REQ_H_
#define _ANALYSE_REQ_H_

#include "comet.pb.h"

//登录信息包
typedef struct tagLoginData
{
    uint64_t id;
    std::string token;
    intptr_t  work_thread;
    intptr_t  bev;
}LoginData;

//转发信息包。
typedef struct tagTransData
{
    uint64_t         from_id;
    uint64_t         to_id;
    intptr_t        work_thread;  //src thread
    int             data_size;
    unsigned char   data[0];
}TransData;

class AnalyseReq
{
    public:
        AnalyseReq();
        ~AnalyseReq();

        bool analyse_data(struct evbuffer *input, struct evbuffer *output);

        bool pack_data(unsigned char *src, const int src_len, const int property_id, unsigned char *des, int &des_len);
        
        bool is_heart();
        void init_data();

        //堆内存 需调用端释放
        unsigned char *get_login_rsp(const bool is_success, const std::string &msg, const int ping_time, int *rsp_len);

        LoginData *get_login_data();
        TransData *get_trans_data();


        //http check
        //生成http post请求数据，data需有足够空间，返回有效数据长度。
        int get_login_post_data(const int64_t id, const std::string &token, const std::string &url, char *data);
        //解析收到的账号验证数据
        bool analyse_login_data(char *data, int len);

        //生成账号关系验证http请求数据
        int get_relation_post_data(const int64_t from_id, const int64_t to_id, const std::string &url, char *data);
        //解析收到的账号关系验证数据
        bool analyse_relation_data(char *data, int len);

    protected:
        bool analyse_pack(struct evbuffer *input, const int head_len, 
                    const int data_head_len, const int data_len);

        bool analyse_login_pack(const unsigned char *data, const int data_size);

        //打包post请求外层包
        int pack_post_data(char *data, char *action, const std::string &url);
        //解析post应答外层包
        bool analyse_post_data(char *data, int len);

    private:
        AnalyseReq(const AnalyseReq&);
        AnalyseReq& operator= (const AnalyseReq&);

    private:
        eglcomet::C2SLogin _req_login;
        eglcomet::S2CLogin _rsp_login;
        eglcomet::OnlineStatus _online_status;

        bool _is_heart;
        LoginData *_login_data;
        TransData *_trans_data;

        //http check
        eglcomet::ReqPacket _req;
        eglcomet::RspPacket _rsp;

        eglcomet::ReqCheckTokenByID _check_id_req;
        eglcomet::RspCheckTokenByID _check_id_rsp;

        eglcomet::ReqCheckRelationByID _check_relation_req;
        eglcomet::RspCheckRelationByID _check_relation_rsp;

        std::string _serialize_str;
};

#endif  //_ANALYSE_REQ_H_
