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
    int64_t id;
    std::string token;
    intptr_t  work_thread;
    intptr_t  bev;
}LoginData;

//转发信息包。
typedef struct tagTransData
{
    int64_t         from_id;
    int64_t         to_id;
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

    protected:
        bool analyse_pack(struct evbuffer *input, const int head_len, 
                    const int data_head_len, const int data_len);

        bool analyse_login_pack(const unsigned char *data, const int data_size);

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
};

#endif  //_ANALYSE_REQ_H_
