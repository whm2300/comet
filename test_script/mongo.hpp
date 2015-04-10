#include "log.h"
#include "mongo/client/dbclient.h"
using namespace mongo;

#include <map>
#include <string>
#include <iostream>

typedef std::map<uint64_t, std::string> IntToStr;

#ifndef verify
#define verify(x) MONGO_verify(x)
#endif

class Mongo
{
    public:
        Mongo(){}
        ~Mongo(){}

        bool open_db(const std::string &ip, const std::string &port)
        {
            std::string err_msg;
            if (!_conn.connect(ip + std::string(":") + port, err_msg)){
                log_info("open mongodb fail:%s", err_msg.c_str());
                return false;
            }
            return true;
        }

        void get_id_token(const char *ns, IntToStr &it_token, int count)
        {
            auto_ptr<DBClientCursor> cursor = _conn.query(ns, BSONObj());
            int i = 0;
            while (cursor->more() && i < count){
                BSONObj obj = cursor->next();
                it_token.insert(std::make_pair<int64_t, std::string>(obj["id"].Int(), obj["token"].str()));
                ++i;
            }
        }

    private:
        DBClientConnection _conn;
};
