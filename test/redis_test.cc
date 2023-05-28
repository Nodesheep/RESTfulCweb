#include "logger.h"
#include "redis.h"

using namespace cweb::db;
using namespace cweb::log;

/*
int main() {
    
    cweb::db::Redis r;
    bool res = r.Connect("127.0.0.1", 8300);
    if(res) {
        std::cout << "连接成功" << std::endl;
    }else {
        std::cout << "连接失败" << std::endl;
    }

    sleep(3);
    r.Cmd("set foo hello");
    r.Cmd("auth 12345678");
    r.Cmd("set foo hello");
    RedisReplyPtr rp = r.Cmd("get foo");
    std::cout << "rp:" << rp->str << std::endl;
    sleep(3);
    
    
    bool res = RedisPoolSingleton::GetInstance()->Init();
    
    if(!res) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "init error");
    }
    
    std::shared_ptr<Redis> redis = RedisPoolSingleton::GetInstance()->GetConnection();
    
    RedisReplyPtr r = redis->Cmd("ping");
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "ping reply: %s", r->str);
    }
    
    r = redis->Cmd("SET %s %s", "foo", "testfoo");
    
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "set reply: %s", r->str);
    }
    
    r = redis->Cmd("SET %s %s", "key_htl_hlea", "first");
    
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "set reply: %s", r->str);
    }
    
    r = redis->Cmd("SET %s %s", "tlyyyy_ghhtl_aseghlea", "second");
    
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "set reply: %s", r->str);
    }
    
    r = redis->Cmd("SET %s %s", "key_htl_hlea_tlyyyy_ghhtl_aseghlea", "third");
    
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "set reply: %s", r->str);
    }
    
    r = redis->Cmd("GET foo");
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "get reply: %s", r->str);
    }
    
    r = redis->Cmd("GET key_htl_hlea");
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "get reply: %s", r->str);
    }
    
    r = redis->Cmd("GET tlyyyy_ghhtl_aseghlea");
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "get reply: %s", r->str);
    }
    
    r = redis->Cmd("GET key_htl_hlea_tlyyyy_ghhtl_aseghlea");
    if(r) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "get reply: %s", r->str);
    }
    
    return 0;
}
*/
