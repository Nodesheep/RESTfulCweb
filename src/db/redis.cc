#include "redis.h"
#include "logger.h"
#include <sys/time.h>
#include <iostream>

using namespace cweb::log;

namespace cweb {
namespace db {

bool Redis::Connect(const std::string& ip, int port, uint64_t ms) {
    timeval tv = {(int)ms / 1000, (int)ms % 1000 * 1000};
    redisContext* c = redisConnectWithTimeout(ip.c_str(), port, tv);

    if(c && !c->err) {
        context_.reset(c, redisFree);
        return true;
    }
    
    std::string err = c ? c->errstr : "";
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis", "connect error : %s", err.c_str());
    
    return false;
}

bool Redis::Auth(const std::string &password) {
    redisReply* r = (redisReply*)redisCommand(context_.get(), "auth %s", password.c_str());
    if(r && r->type != REDIS_REPLY_ERROR) {
        return true;
    }
    
    std::string err = r ? r->str : "";
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis", "cmd auth: %s", err.c_str());
    
    if(r) {
        freeReplyObject(r);
    }
    
    return false;
}

bool Redis::ConnectAndAuth(const std::string &ip, int port, const std::string &password, uint64_t ms) {
    if(Connect(ip, port, ms)) {
        return Auth(password);
    }
    return false;
}

RedisReplyPtr Redis::Cmd(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    redisReply* r = (redisReply*)redisvCommand(context_.get(), fmt, ap);
    va_end(ap);
    
    if(r && r->type != REDIS_REPLY_ERROR) {
        return std::shared_ptr<redisReply>(r, freeReplyObject);
    }
    
    std::string err = r ? r->str : "";
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis", "cmd error: %s", err.c_str());
    
    if(r) {
        freeReplyObject(r);
    }
    
    return nullptr;
}

int Redis::AppendCmd(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int res = redisvAppendCommand(context_.get(), fmt, ap);
    va_end(ap);
    return res;
}

RedisReplyPtr Redis::GetReply() {
    redisReply* r = nullptr;
    if(redisGetReply(context_.get(), (void**)&r) == REDIS_OK) {
        return std::shared_ptr<redisReply>(r, freeReplyObject);
    }
    
    if(r) {
        freeReplyObject(r);
    }
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis", "getreplay error");
    return nullptr;
}

bool RedisCluster::ConnectAndAuth(const std::string& addrs, const std::string &password, uint64_t ms) {
    timeval tv = {(int)ms / 1000, (int)ms % 1000 * 1000};
    redisClusterContext* c = redisClusterContextInit();
    redisClusterSetOptionAddNodes(c, addrs.c_str());
    redisClusterSetOptionTimeout(c, tv);
    redisClusterSetOptionPassword(c, password.c_str());
    
    if (redisClusterConnect2(c) != REDIS_OK) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis cluster", "connect error");
        redisClusterFree(c);
        return false;
    }

    context_.reset(c, redisClusterFree);
    return true;
}

RedisReplyPtr RedisCluster::Cmd(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    redisReply* r = (redisReply*)redisClustervCommand(context_.get(), fmt, ap);
    va_end(ap);

    if(r && r->type != REDIS_REPLY_ERROR) {
        return std::shared_ptr<redisReply>(r, freeReplyObject);
    }
    
    std::string err = r ? r->str : "";
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis cluster", "cmd error: %s", err.c_str());
    
    if(r) {
        freeReplyObject(r);
    }
    
    return nullptr;
}

int RedisCluster::AppendCmd(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int res = redisClustervAppendCommand(context_.get(), fmt, ap);
    va_end(ap);
    return res;
}

RedisReplyPtr RedisCluster::GetReply() {
    redisReply* r = nullptr;
    if(redisClusterGetReply(context_.get(), (void**)&r) == REDIS_OK) {
        return std::shared_ptr<redisReply>(r, freeReplyObject);
    }
    
    if(r) {
        freeReplyObject(r);
    }
    LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis cluster", "getreplay error");
    return nullptr;
}

bool RedisPool::Init() {
    connections_ = std::make_shared<LockfreeQueue<std::shared_ptr<Redis>>>(config_.capcity);
    if(config_.type == RedisConfig::Standalone) {
        for(int i = 0; i < config_.capcity; ++i) {
            std::shared_ptr<Redis> r = std::make_shared<Redis>();
            if(!r->ConnectAndAuth(config_.host, config_.port, config_.password, config_.timeout_ms)) {
                LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis", "redis pool init error");
                return false;
            }
            
            ReleaseConnection(r);
        }
    }else if(config_.type == RedisConfig::Cluster) {
        for(int i = 0; i < config_.capcity; ++i) {
            std::shared_ptr<RedisCluster> r = std::make_shared<RedisCluster>();
            if(!r->ConnectAndAuth(config_.addrs, config_.password, config_.timeout_ms)) {
                LOG(LOGLEVEL_ERROR, CWEB_MODULE, "redis cluster", "redis pool init error");
                return false;
            }
            ReleaseConnection(r);
        }
    }
    return true;
}

}
}
