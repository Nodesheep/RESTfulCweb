#ifndef CWEB_DATABASE_REDIS_H_
#define CWEB_DATABASE_REDIS_H_

#include <string>
#include <hiredis-vip/hiredis.h>
#include <hiredis-vip/hircluster.h>
#include <memory>
#include "dbconnection_pool.h"
#include "cweb_config.h"
#include "singleton.h"

namespace cweb {
namespace db {

typedef std::shared_ptr<redisReply> RedisReplyPtr;

class Redis {
private:
    std::shared_ptr<redisContext> context_;
    
public:
    bool Connect(const std::string& ip, int port, uint64_t ms = 100);
    bool Auth(const std::string& password);
    bool ConnectAndAuth(const std::string& ip, int port, const std::string& password, uint64_t ms = 100);

    virtual RedisReplyPtr Cmd(const char* fmt, ...);
    virtual int AppendCmd(const char* fmt, ...);
    virtual RedisReplyPtr GetReply();
    
    std::string Lock(const std::string& key, uint64_t ms);
    bool Unlock(const std::string& key, const std::string& value);
};

class RedisCluster : public Redis {
private:
    std::shared_ptr<redisClusterContext> context_;
    
public:
    bool ConnectAndAuth(const std::string& addrs, const std::string& password, uint64_t ms = 100);

    virtual RedisReplyPtr Cmd(const char* fmt, ...) override;
    virtual int AppendCmd(const char* fmt, ...) override;
    virtual RedisReplyPtr GetReply() override;
    
    //virtual std::string Lock(const std::string& key, uint64_t ms) override;
    //virtual bool Unlock(const std::string& key, const std::string& value) override;
};


class RedisPool : public DBConnectionPool<Redis> {
private:
    RedisConfig config_;
    
public:
    RedisPool() : DBConnectionPool<Redis>() {}
    bool Init();
};

typedef cweb::util::Singleton<RedisPool> RedisPoolSingleton;

}
}

#endif
